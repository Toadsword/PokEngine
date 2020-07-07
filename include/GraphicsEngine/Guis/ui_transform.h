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
// Date : 20.03.2020
//-----------------------------------------------------------------------------
#pragma once
#pragma once

#include <Math/vector.h>
#include <optional_custom.h>

namespace poke {
namespace graphics {
class UiAnchor {
public:
    inline static const math::Vec2 kLeftTop = math::Vec2(0.0f, 0.0f);
    inline static const math::Vec2 kCenterTop = math::Vec2(0.5f, 0.0f);
    inline static const math::Vec2 kRightTop = math::Vec2(1.0f, 0.0f);
    inline static const math::Vec2 kLeftCenter = math::Vec2(0.0f, 0.5f);
    inline static const math::Vec2 kCenter = math::Vec2(0.5f, 0.5f);
    inline static const math::Vec2 kRightCenter = math::Vec2(1.0f, 0.5f);
    inline static const math::Vec2 kLeftBottom = math::Vec2(0.0f, 1.0f);
    inline static const math::Vec2 kCenterBottom = math::Vec2(0.5f, 1.0f);
    inline static const math::Vec2 kRightBottom = math::Vec2(1.0f, 1.0f);

};

enum UiMargins : uint8_t {
    NONE = 0,
    LEFT = 1,
    RIGHT = 2,
    TOP = 4,
    BOTTOM = 8,
    LEFT_TOP = LEFT | TOP,
    RIGHT_BOTTOM = RIGHT | BOTTOM,
    ALL = LEFT | RIGHT | TOP | BOTTOM
};

/**
 * \brief Class that represents a 2D screen space transform.
 */
class UiTransform {
public:
    /**
     * Creates a new UiTransform.
     */
    UiTransform() = default;

    UiTransform(
        math::Vec2 size,
        math::Vec2 anchor = UiAnchor::kLeftTop,
        math::Vec2 position = {});

    /**
     * Creates a new UiTransform.
     * @param size The size of the rectangle in pixels.
     * @param anchor0 The reference anchor where the rectangle will be placed relative in the parent.
     * @param anchor1 The anchor inside of this rectangle.
     * @param position The pixel offset from the anchor position.
     */
    UiTransform(
        math::Vec2 size,
        math::Vec2 anchor0,
        math::Vec2 anchor1,
        math::Vec2 position);

    UiTransform(
        UiMargins margins,
        math::Vec2 anchor0 = {},
        math::Vec2 anchor1 = {});

    bool operator==(const UiTransform& other) const;
    bool operator!=(const UiTransform& other) const;

    math::Vec2 GetSize() const { return size_; }
    void SetSize(math::Vec2Int size) { size_ = size; }

    math::Vec2 GetAnchor0() const { return anchor0_; }
    void SetAnchor0(math::Vec2 anchor0) { anchor0_ = anchor0; }

    math::Vec2 GetAnchor1() const { return anchor1_; }
    void SetAnchor1(math::Vec2 anchor1) { anchor1_ = anchor1; }

    void SetAnchor(math::Vec2 anchor);

    math::Vec2 GetPosition() const { return position_; }
    void SetPosition(math::Vec2 position) { position_ = position; }

    const std::experimental::optional<UiMargins>& GetMargins() const { return margins_; }
    void SetMargins(const std::experimental::optional<UiMargins>& margins) { margins_ = margins; }

    float GetDepth() const { return depth_; }
    void SetDepth(float depth) { depth_ = depth; }

private:
    math::Vec2 size_;
    math::Vec2 anchor0_;
    math::Vec2 anchor1_;
    math::Vec2 position_;
    std::experimental::optional<UiMargins> margins_;
    float depth_ = 0.0f;
};
} //namespace graphics
} //namespace poke
