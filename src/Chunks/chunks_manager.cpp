#include <Chunks/chunks_manager.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <CoreEngine/engine.h>
#include <bitset>
#include <algorithm>

namespace poke {
namespace chunk {
ChunksManager::ChunksManager(Engine& engine)
{
    engine.GetModuleManager().sceneManager.AddOnUnloadObserver(
        [this]() { this->OnUnloadScene(); });
}

void ChunksManager::OnUnloadScene()
{
    activeChunkIndex_ = 0;

    for (auto i = 0; i < nbChunk_; i++) { chunks_[i] = Chunk(); }

    nbChunk_ = 0;
}

void ChunksManager::RegisterObserverNewActiveChunk(
    const std::function<void(ChunkBitSet)>& observerCallback)
{
    observerNewActiveChunk_.AddObserver(observerCallback);
}

void ChunksManager::UpdateActiveChunkWithPosition(
    const math::Vec3 worldPosition)
{
    //Check if world position is still in active chunk
    if (!chunks_[activeChunkIndex_].IsPositionInside(worldPosition)) {
        //Find new chunk
        for (uint8_t i = 0; i < nbChunk_; i++) {
            if (chunks_[i].IsPositionInside(worldPosition)) {
                SetNewActiveChunk(i);
                break;
            }
        }
    }
}

void ChunksManager::SetActiveChunk(const ChunkIndex chunkIndex) { SetNewActiveChunk(chunkIndex); }

ChunkIndex ChunksManager::CreateChunk() { return nbChunk_++; }

void ChunksManager::DestroyChunk(const ChunkIndex chunkIndex)
{
    for (ChunkIndex i = 0, index = 0; i < nbChunk_; i++) {
        if (i != chunkIndex) {
            auto chunk = chunks_[index];

            //offset chunks to hide
            auto chunksToHide = chunk.GetChunksToHide();
            for (ChunkIndex j = 0, k = 0; j < chunksToHide.size() - 1; j++, k++) {
                if (j == chunkIndex) { k--; }

                chunksToHide[i] = chunksToHide[k];
            }
            chunksToHide.set(chunksToHide.size(), false);
            chunk.SetChunksToHide(chunksToHide);

            //offset chunks to show
            auto chunksToShow = chunk.GetChunksToShow();
            for (ChunkIndex j = 0, k = 0; j < chunksToShow.size() - 1; j++, k++) {
                if (j == chunkIndex) { k--; }

                chunksToShow[i] = chunksToShow[k];
            }
            chunksToShow.set(chunksToShow.size(), false);
            chunk.SetChunksToShow(chunksToShow);

            //offset chunks to destroy
            auto chunksToDestroy = chunk.GetChunksToDestroy();
            for (ChunkIndex j = 0, k = 0; j < chunksToDestroy.size() - 1; j++, k++) {
                if (j == chunkIndex) { k--; }

                chunksToDestroy[i] = chunksToDestroy[k];
            }
            chunksToShow.set(chunksToDestroy.size(), false);
            chunk.SetChunksToDestroy(chunksToDestroy);

            chunks_[i] = chunk;
            index++;
        }
    }

    nbChunk_--;
}

const Chunk& ChunksManager::GetChunk(const ChunkIndex chunkIndex) { return chunks_[chunkIndex]; }

ChunkIndex ChunksManager::GetActiveChunkIndex() { return activeChunkIndex_; }

void ChunksManager::SetChunk(const ChunkIndex chunkIndex, const Chunk& chunk)
{
    cassert(chunkIndex < kMaxChunkNb, "chunkIndex must be smalle th kMaxChunkNb");
    chunks_[chunkIndex] = chunk;
}

void ChunksManager::SetChunks(const std::array<Chunk, kMaxChunkNb>& chunks, const uint8_t nbChunk)
{
    nbChunk_ = nbChunk;
    chunks_ = chunks;
}

ChunkBitSet ChunksManager::AddEntity(const math::Vec3 position)
{
    ChunkBitSet chunksIndex = 0;

    for (ChunkIndex i = 0; i < nbChunk_; i++) {
        if (chunks_[i].IsPositionInside(position)) {
            chunksIndex.set(i);
            return chunksIndex;
        }
    }

    return Chunk::kNullIndex;
}

ChunkBitSet ChunksManager::AddEntity(const physics::AABB aabb)
{
    ChunkBitSet chunksIndex = 0;

    bool found = false;
    for (ChunkIndex i = 0; i < nbChunk_; i++) {
        if (chunks_[i].Overlap(aabb)) {
            chunksIndex.set(i);
            found = true;
        }
    }
    if (!found) { return Chunk::kNullIndex; }

    return chunksIndex;
}

void ChunksManager::DebugDisplayChunk(const ChunkIndex chunkIndex)
{
	auto& ecsManager = EcsManagerLocator::Get();

    for (const auto linkEntity : chunks_[chunkIndex].GetLinkEntities()) {
		ecsManager.SetEntityVisible(linkEntity, ecs::EntityStatus::ACTIVE);
    }
}

void ChunksManager::DebugHideChunk(const ChunkIndex chunkIndex)
{
	auto& ecsManager = EcsManagerLocator::Get();

	for (const auto linkEntity : chunks_[chunkIndex].GetLinkEntities()) {
		ecsManager.SetEntityVisible(linkEntity, ecs::EntityStatus::INACTIVE);
	}
}

ChunkStatus ChunksManager::GetChunkStatus(const ChunkBitSet chunkIndex)
{
    ChunkStatus chunkStatus = ChunkStatus::LENGTH;

    for (ChunkIndex i = 0; i < nbChunk_; i++) {
        if (chunkIndex.test(i)) { chunkStatus = std::min(chunks_[i].GetStatus(), chunkStatus); }
    }

    return chunkStatus;
}

json ChunksManager::ToJson(const std::map<ecs::EntityIndex, ecs::EntityIndex>& baseParentIdToOffsetParentId)
{
    json chunksJson;

    for (ChunkIndex i = 0; i < nbChunk_; i++) { chunksJson[i] = chunks_[i].ToJson(baseParentIdToOffsetParentId); }

    return chunksJson;
}

void ChunksManager::SetFromJson(const json& chunksJson, const std::map<ecs::EntityIndex, ecs::EntityIndex>& baseParentIdToOffsetParentId)
{
    nbChunk_ = chunksJson.size();

    for (size_t i = 0; i < chunksJson.size(); i++) {
        chunks_[i].SetFromJson(chunksJson[i], baseParentIdToOffsetParentId);
        chunks_[i].SetStatus(ChunkStatus::HIDDEN);
    }

    SetNewActiveChunk(0);
}

void ChunksManager::SetNewActiveChunk(const ChunkIndex newActiveChunk)
{
    activeChunkIndex_ = newActiveChunk;

    //Set active entities
    for (const auto entityIndex : chunks_[activeChunkIndex_].GetEntitiesToActivate()) {
        EcsManagerLocator::Get().SetActive(entityIndex, ecs::EntityStatus::ACTIVE);
    }

    //Set visible chunk
    const auto visibleChunksIndex = chunks_[activeChunkIndex_].GetChunksToShow();
    for (ChunkIndex i = 0; i < nbChunk_; i++) {
        if (visibleChunksIndex[i]) { chunks_[i].SetStatus(ChunkStatus::VISIBLE); }
    }

    //Hide chunks
    const auto hiddenChunksIndex = chunks_[activeChunkIndex_].GetChunksToHide();
    for (ChunkIndex i = 0; i < nbChunk_; i++) {
        if (hiddenChunksIndex[i]) { chunks_[i].SetStatus(ChunkStatus::HIDDEN); }
    }

    //Destroy
    const auto destroyedChunksIndex = chunks_[activeChunkIndex_].GetChunksToDestroy();
    for (ChunkIndex i = 0; i < nbChunk_; i++) {
        if (destroyedChunksIndex[i]) {
            chunks_[i].SetStatus(ChunkStatus::DESTROYED);
            chunks_[i].Destroy();
        }
    }

    //Set current chunk active
    chunks_[activeChunkIndex_].SetStatus(ChunkStatus::ACTIVE);

    observerNewActiveChunk_.Notify(activeChunkIndex_);
}
} //namespace chunk
} //namespace poke