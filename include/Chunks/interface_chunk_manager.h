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
// Date : 27.03.2020
//----------------------------------------------------------------------------------
#pragma once
#include <Chunks/chunks.h>

namespace poke {
namespace chunk {
class IChunksManager {
public:
	static const uint8_t kMaxChunkNb = 64;

    IChunksManager() = default;
    virtual ~IChunksManager() = default;

	//------------------------------- OBSERVER --------------------------------
	virtual void RegisterObserverNewActiveChunk(const std::function<void(ChunkBitSet)>& observerCallback) = 0;
    //-------------------------------------------------------------------------

    /**
	 * \brief Create a new chunk
	 * \return the chunk index of the new chunk
	 */
	virtual ChunkIndex CreateChunk() = 0;

    /**
	 * \brief Destroy a given chunk
	 * \details It will keep all chunks continuous in memory and make sure to update existing chunks
	 * \param chunkIndex 
	 */
	virtual void DestroyChunk(ChunkIndex chunkIndex) = 0;

    /**
	 * \brief Update the current active chunk by the given position. This function should be called every frame. 
	 * \param worldPosition 
	 */
	virtual void UpdateActiveChunkWithPosition(math::Vec3 worldPosition) = 0;

    /**
	 * \brief Manually set the current active chunk.
	 * \param chunkIndex 
	 */
	virtual void SetActiveChunk(ChunkIndex chunkIndex) = 0;

    /**
	 * \brief Get a chunk by the given chunkIndex.
	 * \param chunkIndex 
	 * \return 
	 */
	virtual const Chunk& GetChunk(ChunkIndex chunkIndex) = 0;

    /**
	 * \brief Get all chunks
	 * \return 
	 */
	virtual const std::array<Chunk, kMaxChunkNb>& GetChunks() const = 0;

    /**
	 * \brief Set the value for a given chunks.
	 * \param chunkIndex 
	 * \param chunk 
	 */
	virtual void SetChunk(ChunkIndex chunkIndex, const Chunk& chunk) = 0;

    /**
	 * \brief Set all chunks.
	 * \details Warning, it will override every existing chunks data.
	 */
	virtual void SetChunks(const std::array<Chunk, kMaxChunkNb>&chunks, uint8_t nbChunk) = 0;

    /**
	 * \brief Get the active chunk's index.
	 * \return 
	 */
	virtual ChunkIndex GetActiveChunkIndex() = 0;

    /**
	 * \brief Add a new entity inside the chunk manager. Use the world position, the object will be in only one chunk.
	 * \param worldPosition 
	 */
	virtual ChunkBitSet AddEntity(math::Vec3 worldPosition) = 0;

	/**
	 * \brief Add a new entity inside the chunk manager. Use the rigidbody aabb to check all possible chunks.
	 * \param aabb
	 */
	virtual ChunkBitSet AddEntity(physics::AABB aabb) = 0;

    /**
	 * \brief Get the number of chunks in the scene
	 * \return 
	 */
	virtual uint8_t GetChunkNb() const = 0;

    /**
	 * \brief Get the status of a chunk
	 * \param chunkIndex 
	 * \return 
	 */
	virtual ChunkStatus GetChunkStatus(ChunkBitSet chunkIndex) = 0;

    /**
	 * \brief Display a chunk
	 * \details this function must only be used for debugging purpose and is able to set its entities to visible. Must be called every frame.
	 * \param chunkIndex 
	 */
	virtual void DebugDisplayChunk(ChunkIndex chunkIndex) = 0;

	/**
     * \brief Display a chunk
     * \details this function must only be used for debugging purpose and is able to set its entities to visible. Must be called every frame.
     * \param chunkIndex
     */
	virtual void DebugHideChunk(ChunkIndex chunkIndex) = 0;

	virtual json ToJson(const std::map<ecs::EntityIndex, ecs::EntityIndex>& baseParentIdToOffsetParentId) = 0;

	virtual void SetFromJson(const json& chunksJson, const std::map<ecs::EntityIndex, ecs::EntityIndex>& baseParentToOffsetParent) = 0;
};
} //namespace chunk
} //namespace poke
