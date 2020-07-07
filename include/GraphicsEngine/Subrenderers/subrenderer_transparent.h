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
// Date : 17.02.20
//-----------------------------------------------------------------------------
#pragma once
#include <GraphicsEngine/Pipelines/render_pipeline.h>
#include <GraphicsEngine/Buffers/uniform_handle.h>
#include <GraphicsEngine/Models/model_command_buffer.h>
#include <Math/hash.h>

namespace poke {
namespace graphics {
class SubrendererTransparent : public RenderPipeline {
public:
    explicit SubrendererTransparent(Pipeline::Stage stage);
	~SubrendererTransparent();

    void OnRender(const CommandBuffer& commandBuffer) override;

	constexpr static int GetSubrendererIndex()
	{
		return static_cast<int>(SubrendererIndex::TRANSPARENT);
	}
private:
	bool CmdRender(
		const CommandBuffer& commandBuffer,
		ForwardDrawCmd& modelDrawCommand);

    UniformHandle uniformScene_;

    ModelCommandBuffer& modelCmdBuffer_;

	inline static const math::StringHash kProjectionHash =
		math::HashString("projection");

	inline static const math::StringHash kViewHash =
		math::HashString("view");

	inline static const math::StringHash kTransformHash =
		math::HashString("transform");

	inline static const math::StringHash kUboSceneHash =
		math::HashString("UboScene");

	inline static const math::StringHash kUboObjectHash =
		math::HashString("UboObject");
};
} //namespace graphics
} //namespace poke