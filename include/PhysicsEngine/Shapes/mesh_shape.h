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
#include <GraphicsEngine/Models/mesh.h>

namespace poke {
namespace math {
class Transform;
}
}

namespace poke {
namespace physics {
/**
 * \brief Build for using with a mesh as colldier.
 */
class MeshShape final : public IShape {
public:
    MeshShape();
    explicit MeshShape(graphics::Mesh& mesh);

    bool operator==(const MeshShape& rhs) const;

    bool operator!=(const MeshShape& rhs) const;

    AABB ComputeAABB(
        math::Vec3 position,
        math::Vec3 scale,
        math::Vec3 rotation) const override;

	AABB ComputeAABB(const math::Transform& transform) const override;

    ShapeType GetType() const override;

    std::unique_ptr<IShape> Clone() override;

    math::Vec3 GetMeshExtent() const;

    void SetMeshExtent(math::Vec3 meshExtent);

    math::Vec3 GetOffset() const;

    void SetOffset(math::Vec3 offset);

    void SetFromJson(json shapeJson) override;
    json ToJson() const override;
private:
	math::Vec3 meshExtent_;
	math::Vec3 positionOffset_;
};
} //namespace physics
} //namespace poke
