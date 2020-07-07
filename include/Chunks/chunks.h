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
// Author : Nicolas Schneider
// Co-Author :
// Date : 26.30.2020
//----------------------------------------------------------------------------------
#pragma once

#include <vector>
#include <bitset>

#include <Math/vector.h>
#include <Ecs/ComponentManagers/transforms_manager.h>

namespace poke {
namespace chunk {
using ChunkBitSet = std::bitset<64>;
using ChunkIndex = uint8_t;

//Indicate the status of a chunk, the enum is in order od priority.
enum class ChunkStatus : uint8_t {
    ACTIVE = 0,
    VISIBLE,
    HIDDEN,
    DESTROYED,
    LENGTH
};

/**
 * \brief Representation of a portion of space to cut the level. It's used to store entities and to (un)activate them if the chunk is active or not
 */
class Chunk {
public:
    inline static const ChunkBitSet kNullIndex = -1;

    /**
	 * \brief This constructor should be used to construct a chunk when their is no chunk in the scene.
	 */
    Chunk();

    //--------------------------------- JSON ----------------------------------
    json ToJson(const std::map<ecs::EntityIndex, ecs::EntityIndex>& baseParentIdToOffsetParentId) const;

    void SetFromJson(const json& json, const std::map<ecs::EntityIndex, ecs::EntityIndex>& baseParentIdToOffsetParentId);
    //-------------------------------------------------------------------------

    //--------------------------------- LOGIC ---------------------------------
    bool IsTransformInside(const math::Transform& transform) const;

    bool Overlap(physics::AABB aabb) const;

    bool IsEntityInside(ecs::EntityIndex entity, math::Vec3 position) const;

    bool IsPositionInside(math::Vec3 position) const;

    /**
     * \brief Add a link entity
     * \details The given entity will be display depending on the status of this chunk
     * \param entity 
     */
    void AddLinkEntity(ecs::EntityIndex entity);
    /**
     * \brief Remove a link entity
     * \param entityIndex 
     */
    void RemoveLinkEntity(ecs::EntityIndex entityIndex);

    /**
	 * \brief Add an entity to activate
	 * \details The given entity will be activated when entering the chunk
	 * \param entity 
	 */
    void AddEntityToActivate(ecs::EntityIndex entity);
    /**
	 * \brief Remove an entity to active
	 * \param entityIndex 
	 */
    void RemoveEntityToActivate(ecs::EntityIndex entityIndex);

    /**
     * \brief Add a chunk to show
     * \details The given chunk will be show when entering this chunk
     * \param chunkIndex 
     */
    void AddChunkToShow(ChunkIndex chunkIndex);
    /**
     * \brief Remove a chunk to show
     * \param chunkIndex 
     */
    void RemoveChunkToShow(ChunkIndex chunkIndex);

    /**
     * \brief Add a chunk to hide
     * \details The given chunk will be hidden when entering this chunk
     * \param chunkIndex 
     */
    void AddChunkToHide(ChunkIndex chunkIndex);
    /**
     * \brief Remove a chunk to hide
     * \param chunkIndex 
     */
    void RemoveChunkToHide(ChunkIndex chunkIndex);

    /**
     * \brief Add a chunk to destroy
     * \details The given chunk will be destroyed when entering this chunk
     * \param chunkIndex 
     */
    void AddChunkToDestroy(ChunkIndex chunkIndex);
    /**
     * \brief Remove a chunk to destroy
     * \param chunkIndex 
     */
    void RemoveChunkToDestroy(ChunkIndex chunkIndex);

    void Destroy();
    //-------------------------------------------------------------------------

    //--------------------------- GETTER / SETTER -----------------------------
    math::Vec3 GetPosition() const { return position_; }
    void SetPosition(const math::Vec3 position) { position_ = position; }

    math::Vec3 GetExtent() const { return extent_; }
    void SetExtent(const math::Vec3 extends) { extent_ = extends; }

    const ChunkBitSet GetChunksToShow() const { return chunksToShow_; }
    void SetChunksToShow(const ChunkBitSet chunkToShow) { chunksToShow_ = chunkToShow; }

    const ChunkBitSet GetChunksToHide() const { return chunksToHide_; }
    void SetChunksToHide(const ChunkBitSet chunkToHide) { chunksToHide_ = chunkToHide; }

    const std::vector<ecs::EntityIndex>& GetEntitiesToActivate() const;
    void SetEntitiesToActivate(const std::vector<ecs::EntityIndex>& entitiesToActivate);

    const std::vector<ecs::EntityIndex>& GetLinkEntities() const;
    void SetLinkEntities(const std::vector<ecs::EntityIndex>& linkEntities);

    const ChunkBitSet GetChunksToDestroy() const { return chunksToDestroy_; }
    void SetChunksToDestroy(const ChunkBitSet chunkToDestroy) { chunksToDestroy_ = chunkToDestroy; }

    ChunkStatus GetStatus() const { return chunkStatus_; }
    void SetStatus(const ChunkStatus chunkStatus) { chunkStatus_ = chunkStatus; }
    //-------------------------------------------------------------------------

private:
    math::Vec3 position_;
    math::Vec3 extent_;

    ChunkStatus chunkStatus_;

    ChunkBitSet chunksToShow_;
    ChunkBitSet chunksToHide_;
    ChunkBitSet chunksToDestroy_;
    std::vector<ecs::EntityIndex> entitiesToActivate_;

    std::vector<ecs::EntityIndex> linkEntities_;
};
} //namespace chunk
} //namespace poke
