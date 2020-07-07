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
// Date : 04.02.20
//----------------------------------------------------------------------------------
#pragma once
#include <Chunks/interface_chunk_manager.h>
#include <bitset>
#include <algorithm>

namespace poke {
namespace chunk {
class NullChunksManager : public IChunksManager {
public:
    NullChunksManager() {}
    ~NullChunksManager() = default;

    //------------------------------- OBSERVER --------------------------------
    virtual void RegisterObserverNewActiveChunk(
        const std::function<void(ChunkBitSet)>& observerCallback) override
    {
        observerCallback(0);
    };
    //-------------------------------------------------------------------------

    void UpdateActiveChunkWithPosition(math::Vec3 worldPosition) override
    {
		worldPosition;
    }
    void SetActiveChunk(ChunkIndex chunkIndex) override
    {
		chunkIndex;
    }

    const ::poke::chunk::Chunk& GetChunk(ChunkIndex chunkIndex) override
    {
		chunkIndex;
        cassert(false, "Impossible to get chunk from null_chunk_manager");
		abort();
    }

    ChunkIndex GetActiveChunkIndex() override { return 0; }

	const std::array<Chunk, kMaxChunkNb>& GetChunks() const override
    {
		std::array<Chunk, kMaxChunkNb> dummy;
        return dummy;
    }

    ChunkBitSet AddEntity(math::Vec3 position) override
    {
		position;
		return 0;
    }

    ChunkBitSet AddEntity(physics::AABB aabb) override
    {
		aabb;
		return {};
    }

    json ToJson(const std::map<ecs::EntityIndex, ecs::EntityIndex>& baseParentIdToOffsetParentId) override {
		baseParentIdToOffsetParentId;
        return json();
    }
    void SetFromJson(const json& chunksJson, const std::map<ecs::EntityIndex, ecs::EntityIndex>& baseParentIdToOffsetParentId) override
    {
		chunksJson;
    }

	uint8_t GetChunkNb() const override { return 0; }
	ChunkStatus GetChunkStatus(ChunkBitSet chunkIndex) override { return ChunkStatus::HIDDEN; }
    ChunkIndex CreateChunk() override { return 0; }
    void DestroyChunk(ChunkIndex chunkIndex) override{}
    void SetChunk(ChunkIndex chunkIndex, const Chunk& chunk) override{}
    void SetChunks(const std::array<Chunk, kMaxChunkNb>&chunks, uint8_t nbChunk) override{}
    void DebugDisplayChunk(ChunkIndex chunkIndex) override
    {
		chunkIndex;
		void();
    }
    void DebugHideChunk(ChunkIndex chunkIndex) override
    {
		chunkIndex;
		void();
    }
};
} //namespace chunk
} //namespace poke
