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
// Date : 11.02.20
//-----------------------------------------------------------------------------
#pragma once

#include <GraphicsEngine/Pipelines/pipeline_material.h>
#include <GraphicsEngine/Models/material_export_data.h>

#include <xxhash.h>

namespace poke {
namespace graphics {

enum class MaterialType : uint8_t {
	DIFFUSE = 0,
    SKYBOX,
    TRAIL,
    PARTICLE,
    LENGTH
};

/**
 * \brief Represent a material that can be used by any object, if an object what to be drawn, it needs to have a material
 */
class Material {
public:
	enum class RenderMode : uint8_t {
		OPAQUE,
		TRANSPARENT,
		LENGTH
	};

	static std::string RenderModeToString(const RenderMode renderMode)
	{
		switch (renderMode) {
		case RenderMode::OPAQUE:
			return "Opaque";
		case RenderMode::TRANSPARENT:
			return "Transparent";
		case RenderMode::LENGTH: break;
		default:;
		}
		return "";
	}

	static RenderMode StringToRenderMode(const std::string& renderModeName)
	{
		if (renderModeName == "Opaque") { return RenderMode::OPAQUE; }
		if (renderModeName == "Transparent") { return RenderMode::TRANSPARENT; }
		return RenderMode::LENGTH;
	}

	using PushDataContainer = std::vector<std::pair<XXH64_hash_t, MaterialExportData>>;

	explicit Material() = default;

    virtual ~Material() = default;

    virtual void CreatePipeline(const VertexInput& vertexInput) = 0;

    const PushDataContainer& ExportUniformData() const { return uniformData_; }

    const PushDataContainer& ExportDescriptorData() const;

    const PipelineMaterial& GetPipelineMaterial() const;

    bool BindPipeline(const CommandBuffer& commandBuffer) const;

	virtual MaterialType GetType() const = 0;

	virtual json ToJson() const = 0;

	virtual void SetFromJson(const json& materialJson) = 0;

	virtual RenderMode GetRenderMode() const = 0;
	virtual void SetRenderMode(const RenderMode renderMode) = 0;

protected:
    std::experimental::optional<PipelineMaterial&> pipelineMaterial_ = std::
        experimental::nullopt;

    PushDataContainer uniformData_;
    PushDataContainer descriptorData_;

	RenderMode renderMode_ = RenderMode::OPAQUE;
};
} //namespace graphics
} //namespace poke
