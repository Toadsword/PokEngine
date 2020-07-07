//----------------------------------------------------------------------------------
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
// Author : Robin Alves
// Co-Author : Nicolas Schneider
// Date : 17.02.2020
//----------------------------------------------------------------------------------
#pragma once

#include <Math/vector.h>
#include <Math/matrix.h>

namespace poke {
namespace math {

enum TransformDirtyFlagStatus : uint8_t{
	IS_LOCAL_DIRTY = 1 << 0,
	IS_WORLD_DIRTY = 1 << 1
};

using TransformDirtyFlag = uint8_t;

class Transform {
public:
    explicit Transform(
        Vec3 position = Vec3(0, 0, 0),
        Vec3 rotation = Vec3(0, 0, 0),
        Vec3 scale = Vec3(1, 1, 1));

	~Transform() = default;

    bool operator==(const Transform& other) const;

    bool operator!=(const Transform& other) const;

    Vec3 GetLocalPosition() const;

	void SetLocalPosition(Vec3 value);

	Vec3 GetLocalRotation() const;

	void SetLocalRotation(Vec3 value);

	Vec3 GetLocalScale() const;

	void SetLocalScale(Vec3 value);

	/**
	 * \brief Get a json with all the data from the transform
	 */
	const json ToJson() const;

	/**
	 * \brief Set all the data from a json
	 */
	void SetFromJson(const json& transformJson);

	/**
	 * \brief This function should only be used when the transform has no parent and its localSpace is equal to its worldSpace
	 * \return
	 */
	Matrix4 GetWorldMatrix() const;
private:

	Vec3 localPosition_;
	Vec3 localRotation_;
	Vec3 localScale_;
};
}
} //namespace poke::math
