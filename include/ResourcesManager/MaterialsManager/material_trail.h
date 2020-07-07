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
// Date : 29.03.2020
//-----------------------------------------------------------------------------
#pragma once
#include <GraphicsEngine/Models/material.h>
#include <Math/hash.h>

namespace poke {
/**
 * \brief represents the default material that use can be used by PBR
 */
class MaterialTrail: public graphics::Material {
public:
    /**
     * \brief
     * \param color
     * \param textureBaseColor
     */
    explicit MaterialTrail(
        Color color = Color::white,
        std::experimental::optional<const graphics::Image2d&> textureBaseColor = std::experimental::nullopt
    );

    bool operator==(const MaterialTrail& other) const;

    bool operator!=(const MaterialTrail& other) const;

    void CreatePipeline(const graphics::VertexInput& vertexInput) override;

    Color GetColor() const { return color_; }
    void SetColor(const Color& color);

    std::experimental::optional<const graphics::Image2d&> GetTextureBaseColor() const;
    void SetTextureBaseColor(const graphics::Image2d& textureBaseColor);
    void ResetTextureBaseColor();

    graphics::MaterialType GetType() const override;

    RenderMode GetRenderMode() const override { return renderMode_; }
    void SetRenderMode(RenderMode renderMode) override;

    json ToJson() const override;

    void SetFromJson(const json& materialJson) override;
private:
    void ResetPipeline();

    Color color_;

    std::experimental::optional<const graphics::Image2d&> textureBaseColor_;

    inline static const math::StringHash kSamplerBaseColorHash = math::HashString("samplerBaseColor");
    inline static const math::StringHash kColorHash = math::HashString("color");
};
} //namespace poke
