//-----------------------------------------------------------------------------
// Copyright (c) 2019-2020, POK Family. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of POK Family nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author : Nicolas Schneider
// Co-Author :
// Date : 20.01.20
//-----------------------------------------------------------------------------
#pragma once

#include <map>
#include <unordered_map>

#include <GraphicsEngine/Pipelines/render_pipeline.h>

namespace poke {
namespace graphics {
class CommandBuffer;

class SubrendererContainer {
public:
    SubrendererContainer() = default;

    SubrendererContainer(const SubrendererContainer&) = delete;

    SubrendererContainer& operator=(const SubrendererContainer&) = delete;

    ~SubrendererContainer() { /*stages_.clear(); */};

	/**
     * Gets a Subrender.
     * @tparam T The Subrender type.
     * @return The Subrender.
     */
	template<typename T>
	T &Get() const {
		static_assert(HasGetSubrendererIndex<T>::value, "T has to define a function called GetSubrendererIndex");
		static_assert(std::is_base_of<RenderPipeline, T>::value, "T has to be a subset of RenderPipeline");

		auto typeId = T::GetSubrendererIndex();

		auto it = subrenderers_.find(typeId);

		cassert(it != subrenderers_.end() && it->second, "Error when trying to access subrenderer");
		
		return static_cast<T&>(*it->second);
	}

    /**
     * \brief Add a new subrenderer to the container.
     * \tparam T 
     * \tparam Args 
     * \param subrenderer
     */
	template<typename T, typename... Args>
	void Add(const Pipeline::Stage &stage, std::unique_ptr<T> &&subrenderer) {

		static_assert(HasGetSubrendererIndex<T>::value, "T has to define a function called GetSubrendererIndex");
		static_assert(std::is_base_of<RenderPipeline, T>::value, "T has to be a subset of RenderPipeline");

	    auto typeId = T::GetSubrendererIndex();

		// Insert the stage value
		stages_.insert({ StageIndex(stage, subrenderers_.size()), typeId });

		// Then, add the Subrender
		subrenderers_[typeId] = std::move(subrenderer);
	}

	/**
	 * \brief Iterates through all Subrenders.
	 * \param stage The Subrender stage.
	 * \param commandBuffer The command buffer to record render command into.
	 */
	void RenderStage(const Pipeline::Stage &stage, const CommandBuffer &commandBuffer)
	{
		for (const auto &[stageIndex, typeId] : stages_) {
			if (stageIndex.first != stage) {
				continue;
			}

			if (auto &subrenderer = subrenderers_[typeId]) {
				if (subrenderer->IsEnabled()) {
					subrenderer->OnRender(commandBuffer);
				}
			}
		}
	}


private:
	using StageIndex = std::pair<Pipeline::Stage, std::size_t>;

	std::unordered_map<int, std::unique_ptr<RenderPipeline>> subrenderers_;

	std::multimap<StageIndex, int> stages_;
};
} //namespace graphics
} //namespace poke
