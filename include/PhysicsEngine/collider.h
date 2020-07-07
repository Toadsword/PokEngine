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
// Date : 01.02.20
//-----------------------------------------------------------------------------
#pragma once
#include <Math/vector.h>
#include <PhysicsEngine/Shapes/interface_shape.h>
#include <PhysicsEngine/Shapes/box_shape.h>
#include <PhysicsEngine/Shapes/ellipsoid_shape.h>
#include <PhysicsEngine/Shapes/mesh_shape.h>
#include <PhysicsEngine/Shapes/sphere_shape.h>

namespace poke {
namespace math {
class Transform;
}
}

namespace poke {
namespace physics {

/**
 * \brief Physical collider
 */
struct Collider {
    Collider();
	~Collider();

    bool operator==(const Collider& other) const;

    bool operator!=(const Collider& other) const;

    Collider(const Collider& other);

    Collider(Collider&& other) noexcept;

    Collider& operator=(const Collider& other);

    Collider& operator=(Collider&& other) noexcept;


    /**
     * \brief Set the shape to be a box and copy data from the given boxShape.
     * \param boxShape 
     */
    void SetShape(BoxShape boxShape);

    /**
     * \brief Set the shape to be a sphere and copy data from the given sphereShape.
     * \param sphereShape 
     */
    void SetShape(SphereShape sphereShape);

    /**
     * \brief Set the shape to be a mesh and copy data from the given meshShape.
     * \param meshShape 
     */
    void SetShape(MeshShape meshShape);

    /**
     * \brief Set the shape to be a ellipsoid and copy data from the given ellipsoidShape.
     * \param ellipsoidShape 
     */
    void SetShape(EllipsoidShape ellipsoidShape);

    /**
     * \brief Generate an AABB.
     * \param center 
     * \param extent 
     * \param rotation 
     * \return 
     */
    AABB ComputeAABB(
        math::Vec3 center = {},
        math::Vec3 extent = {},
        math::Vec3 rotation = {}) const;

    /**
	 * \brief Generate an AABB.
	 * \param transform 
	 * \return 
	 */
	AABB ComputeAABB(
		const math::Transform& transform) const;

    json ToJson() const;

    void SetFromJson(const json& json);

    union {
		BoxShape boxShape;
		EllipsoidShape ellipsoidShape;
		MeshShape meshShape;
		SphereShape sphereShape;
    };
    ShapeType shapeType;
    bool isTrigger;
};

} //namespace physics
} //namespace poke
