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
// Date : 11.03.2020
//-----------------------------------------------------------------------------
#pragma once

#include <GraphicsEngine/Pipelines/pipeline.h>
#include <GraphicsEngine/Commands/command_buffer.h>
#include <GraphicsEngine/Pipelines/render_pipeline.h>
#include <GraphicsEngine/Posts/filter_blur.h>
#include <GraphicsEngine/Posts/filter_bloom.h>
#include <GraphicsEngine/Devices/window.h>

namespace poke {
namespace graphics {
/**
 * \brief Pipeline specific to create a compute pass
 */
class PipelineBlur : public RenderPipeline {
public:
    
    explicit PipelineBlur(const Pipeline::Stage &pipelineStage, float blur = 2.0f);

	void OnRender(const CommandBuffer &commandBuffer) override;

	float GetBlur() const { return blur_; }
	void SetBlur(float blur) { blur_ = blur; }

	constexpr static int GetSubrendererIndex()
	{
		return static_cast<int>(SubrendererIndex::FILTER_BLUR);
	}
private:

	void OnWindowResize();
	const Window& kWindow_;

	FilterBlur filterBlurVertical_;
	FilterBlur filterBlurHorizontal_;
	FilterBloom bloom_;

	std::unique_ptr<Image2d> output_;
	std::unique_ptr<Image2d> output1_;

	float blur_;

};
} // namespace graphics
} // namespace poke
