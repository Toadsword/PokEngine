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
#include <GraphicsEngine/Models/material.h>
#include <GraphicsEngine/Images/image_cube.h>
#include <Utility/color.h>
#include <json.hpp>
#include <Math/hash.h>

namespace poke {
class MaterialSkybox : public graphics::Material {
public:
    /**
     * \brief 
     * \param texture 
     * \param colorDiffuse 
     */
    explicit MaterialSkybox(
        std::experimental::optional<const graphics::ImageCube&> texture = std::
            experimental::nullopt,
        Color colorDiffuse = Color::white);

    bool operator==(const MaterialSkybox& other) const;

    bool operator!=(const MaterialSkybox& other) const;

    void CreatePipeline(const graphics::VertexInput& vertexInput) override;

    std::experimental::optional<const graphics::ImageCube&> GetTexture() const { return texture_; }
    void SetTexture(const graphics::ImageCube& texture);

    Color GetColorDiffuse() const { return colorDiffuse_; }
    void SetColorDiffuse(Color colorDiffuse);

    float GetBlend() const { return blend_; }
    void SetBlend(float blend);

	json ToJson() const override;

	void SetFromJson(const json& materialJson) override;

    graphics::MaterialType GetType() const;


	RenderMode GetRenderMode() const override { return renderMode_; }
	void SetRenderMode(const RenderMode renderMode) override { renderMode_ = renderMode; }
private:
    std::experimental::optional<const graphics::ImageCube&> texture_;

    Color colorDiffuse_;
    float blend_;

	inline static const math::StringHash kSamplerColorHash =
		math::HashString("samplerColor");

	inline static const math::StringHash kBaseColorHash =
		math::HashString("baseColor");

	inline static const math::StringHash kBlendFactorHash =
		math::HashString("blendFactor");
};
} // namespace poke
