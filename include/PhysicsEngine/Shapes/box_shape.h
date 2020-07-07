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
#include <PhysicsEngine/Shapes/interface_shape.h>
#include <Math/matrix.h>

namespace poke {
namespace math {
class Transform;
}
}

namespace poke {
namespace physics {
class BoxShape final : public IShape {
public:
    explicit BoxShape(
        math::Vec3 positionOffset = {},
        math::Vec3 extent = {});
    virtual ~BoxShape() = default;

    bool operator==(const BoxShape& other) const;

    bool operator!=(const BoxShape& other) const;

    AABB ComputeAABB(
        math::Vec3 position = {},
        math::Vec3 scale = {},
        math::Vec3 rotation = {}) const override;

	AABB ComputeAABB(const math::Transform& transform) const override;

    ShapeType GetType() const override;

    std::unique_ptr<IShape> Clone() override;

    math::Vec3 GetCenter() const;

    void SetCenter(math::Vec3 center);

    math::Vec3 GetExtent() const;

    void SetExtent(math::Vec3 extent);

    void SetFromJson(json shapeJson) override;

	json ToJson() const override;

private:
    math::Matrix3 GetRotationMatrix(math::Vec3 theta) const;

    math::Vec3 positionOffset_;
    math::Vec3 extent_;

    inline static const std::array<math::Vec3, 4> kVertices = { 
        math::Vec3(-1.0f, -1.0f, 1.0f),
        math::Vec3(-1.0f, 1.0f, 1.0f),
        math::Vec3(-1.0f, -1.0f, -1.0f),
        math::Vec3(-1.0f, 1.0f, -1.0f)
    };
};
} //namespace physics
} //namespace poke