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
// Date :  19.11.19
//----------------------------------------------------------------------------------
#pragma once
#include <cstdint>

namespace poke {
namespace ecs {

using ComponentMask = uint32_t;
const ComponentMask kNoEntity = 0;

namespace ComponentType { 
enum ComponentType : ComponentMask {
	EMPTY = 0u,
    TRANSFORM = 1u << 0,
    RIGIDBODY = 1u << 1,
    COLLIDER = 1u << 2,
    MODEL = 1u << 3,
    LIGHT = 1U << 4,
	SPLINE_FOLLOWER = 1u << 5,
    PARTICLE_SYSTEM = 1u << 6,
    AUDIO_SOURCE = 1u << 7,
    TRAIL_RENDERER = 1u << 8,
    SEGMENT_RENDERER = 1u << 9,
    EDITOR_COMPONENT = 1u << 10,

    ENEMY = 1u << 11,
	PLAYER = 1u << 12,
	DESTRUCTIBLE_ELEMENT = 1u << 13,
	WEAPON = 1u << 14,
	PROJECTILE = 1u << 15,
	MISSILE = 1u << 16,
	SPECIAL_ATTACK = 1u << 17,
	SPLINE_STATES = 1u << 18,
	JIGGLE = 1u << 19,
	GAME_CAMERA = 1u << 20,
    LENGTH = 1u << 21
};
}//namespace ComponentType

namespace EntityFlag { 
enum EntityFlag : ComponentMask {
	IS_ACTIVE = 1u << 22,
	IS_VISIBLE = 1u << 23,
	TAG_START = 1u << 24,
	TAG_END = 1u << 25
};
}//namespace EntityFlag

enum class EntityStatus : uint8_t {
    INACTIVE = 0,
    ACTIVE = 1
};

using EntityIndex = int;
using EntityTag = uint8_t;

/**
 * \brief Represents an entity pool. Both value are included in the pool.
 */
struct EntityPool {
    /**
	 * \brief Constructor for an entityPool.
	 * \param firstEntity : It's the first entity of the pool (the smallest). It must be included inside the pool.
	 * \param lastEntity : It's the last entity of the pool (the biggest). It must be included inside the pool.
	 */
	explicit EntityPool(const EntityIndex firstEntity = 0, const EntityIndex lastEntity = 0) :
        firstEntity(firstEntity),
        lastEntity(lastEntity),
        nextFreeEntity(firstEntity){
	}

	EntityIndex firstEntity;
	EntityIndex lastEntity;

	EntityIndex nextFreeEntity;
};

using ArchetypeID = int;
inline static const ArchetypeID defaultArchetypeID = 0;

//Flag
const short kFlagLength = 5;
const short kMaxFlagValue = 31;

//Parent
const int kNoParent = -1;

//Entitysize
const int kEntityBaseSize = 1500; //TODO(@Nico) Change entity max size to size_t instead of int.

const int kArchetypeStartBeacon = 100;
const float kEntityResizeFactor = 1.5f;
} //namespace ecs
} //namespace poke
