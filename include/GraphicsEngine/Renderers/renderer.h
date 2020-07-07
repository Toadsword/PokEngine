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
// Date : 29.01.20
//-----------------------------------------------------------------------------
#pragma once

#include <GraphicsEngine/Subrenderers/subrenderer_container.h>
#include <GraphicsEngine/RenderStage/render_stage.h>

namespace poke {
namespace graphics {
/**
 * \brief A renderer contain all pass and subpass.
 */
class Renderer {
public:
    /**
     * \brief 
     * \param started 
     */
    explicit Renderer(const bool started = false) : started_(started) { }

    virtual ~Renderer() = default;

    virtual void OnEngineInit() = 0;

    SubrendererContainer& GetRendererContainer() { return rendererContainer_; }

    bool HasStarted() const { return started_; }
    virtual void Start() = 0;

	/**
	 * \brief Adds a Subrender.
	 * \tparam T The Subrender type.
	 * \param pipelineStage The Subrender pipeline stage.
	 * \tparam Args The constructor arg types.
	 * \param args The constructor arguments.
	 */
	template<typename T, typename... Args>
	void AddSubrenderer(const Pipeline::Stage &pipelineStage, Args &&...args) {
		rendererContainer_.Add<T>(pipelineStage, std::make_unique<T>(pipelineStage, std::forward<Args>(args)...));
	}

	void AddRenderStage(std::unique_ptr<RenderStage> &&renderStage) {
		renderStages_.emplace_back(std::move(renderStage));
	}

	RenderStage& GetRenderStage(const uint32_t index) const {
		cassert(!renderStages_.empty() && renderStages_.size() > index, "error while accession render stage");

		return *renderStages_.at(index);
	}

	std::vector<std::unique_ptr<RenderStage>>& GetRenderStages() {
		return renderStages_;
	}
protected:
    bool started_;
    SubrendererContainer rendererContainer_;
	std::vector<std::unique_ptr<RenderStage>> renderStages_;
};
} //namespace graphics 
} //namespace poke
