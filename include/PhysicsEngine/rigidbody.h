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

namespace poke {
namespace physics {
/**
 * \brief All rigidbody types.
 */
enum class RigidbodyType : uint8_t {
    DYNAMIC = 0,
    KINEMATIC,
    STATIC,
};

/**
 * \brief Main physics object. An entity need a rigidbody to be updated by the physics engine.
 */
struct Rigidbody {

    bool operator==(const Rigidbody& other) const;
    bool operator!=(const Rigidbody& other) const;

    /**
     * \brief Get a json with all the data from the rigidbody
     */
	const json ToJson() const;

	/**
	 * \brief Set all the data from a json
	 */
	void SetFromJson(const json& transformJson);

    math::Vec3 linearVelocity;
    math::Vec3 angularVelocity;

    float linearDrag = 1;
    float angularDrag = 1;

    RigidbodyType type = RigidbodyType::DYNAMIC;
};

inline static const Rigidbody kEmptyRigidbody{
    math::Vec3(0),
    math::Vec3(0),
    0,
    0,
    RigidbodyType::STATIC
};

} //namespace physics
} //namespace poke
