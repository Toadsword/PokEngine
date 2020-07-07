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
// Date : 26.03.2020
//----------------------------------------------------------------------------------
#pragma once
#include <bitset>

#include <Chunks/interface_chunk_manager.h>
#include <optional_custom.h>
#include <CoreEngine/Observer/subjects_container.h>

namespace poke {
class Engine;

namespace chunk {
class ChunksManager : public IChunksManager {
public:
    ChunksManager(Engine& engine);
    ~ChunksManager() = default;

    //------------------------------- OBSERVER --------------------------------
    void RegisterObserverNewActiveChunk(
        const std::function<void(ChunkBitSet)>& observerCallback) override;
    //-------------------------------------------------------------------------

    //-------------------------------- CHUNK ----------------------------------
    void UpdateActiveChunkWithPosition(math::Vec3 worldPosition) override;

    void SetActiveChunk(ChunkIndex chunkIndex) override;

    ChunkIndex CreateChunk() override;

    void DestroyChunk(ChunkIndex chunkIndex) override;

    const Chunk& GetChunk(ChunkIndex chunkIndex) override;

    ChunkIndex GetActiveChunkIndex() override;

    void SetChunk(ChunkIndex chunkIndex, const Chunk& chunk) override;

    void SetChunks(const std::array<Chunk, kMaxChunkNb>& chunks, uint8_t nbChunk) override;

    const std::array<Chunk, kMaxChunkNb>& GetChunks() const override { return chunks_; }

    uint8_t GetChunkNb() const override { return nbChunk_; }

    ChunkStatus GetChunkStatus(ChunkBitSet chunkIndex) override;

    //-------------------------------------------------------------------------

    //------------------------------- ENTITIES --------------------------------
    ChunkBitSet AddEntity(math::Vec3 position) override;

    ChunkBitSet AddEntity(physics::AABB aabb) override;
    //-------------------------------------------------------------------------

	//------------------------------- ENTITIES --------------------------------
    void DebugDisplayChunk(ChunkIndex chunkIndex) override;

    void DebugHideChunk(ChunkIndex chunkIndex) override;
	//-------------------------------------------------------------------------

    //--------------------------------- JSON ----------------------------------
    json ToJson(const std::map<ecs::EntityIndex, ecs::EntityIndex>& baseParentIdToOffsetParentId) override;

    void SetFromJson(const json& chunksJson, const std::map<ecs::EntityIndex, ecs::EntityIndex>& baseParentIdToOffsetParentId) override;
    //-------------------------------------------------------------------------

private:
    void SetNewActiveChunk(ChunkIndex newActiveChunk);

    void OnUnloadScene();

    std::array<Chunk, kMaxChunkNb> chunks_;
    ChunkIndex nbChunk_ = 0;

    ChunkIndex activeChunkIndex_ = 0;

    std::experimental::optional<const math::Transform> targetTransform_;

    observer::Subject<ChunkBitSet> observerNewActiveChunk_;
};
} //namespace chunk
} //namespace poke
