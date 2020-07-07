#include <Chunks/chunks.h>

#include <algorithm>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Chunks/interface_chunk_manager.h>

namespace poke {
namespace chunk {
Chunk::Chunk()
    : position_({0, 0, 0}),
      extent_({0, 0, 0}) { }

json Chunk::ToJson(const std::map<ecs::EntityIndex, ecs::EntityIndex>& baseParentIdToOffsetParentId) const
{
    json newJson;

    //Position
    newJson["position"] = position_.ToJson();

    //Scale
    newJson["size"] = extent_.ToJson();

    //Linked entities
    newJson["gameObjects"] = nlohmann::detail::value_t::array;
    for (size_t i = 0; i < linkEntities_.size(); ++i) {
        newJson["gameObjects"][i] = baseParentIdToOffsetParentId.at(linkEntities_[i]);
    }

    //Entity to set active
    newJson["entitiesIdToActivate"] = nlohmann::detail::value_t::array;
    for (size_t i = 0; i < entitiesToActivate_.size(); i++) {
        newJson["entitiesIdToActivate"][i] = baseParentIdToOffsetParentId.at(entitiesToActivate_[i]);
    }

    //Chunks to show
    int nb = 0;
    newJson["chunksToShow"] = nlohmann::detail::value_t::array;
    for (int i = 0; i < IChunksManager::kMaxChunkNb; i++) {
        if (chunksToShow_[i]) {
            newJson["chunksToShow"][nb] = i;
            nb++;
        }
    }

    //Chunks to destroy
    nb = 0;
    newJson["chunksToDestroy"] = nlohmann::detail::value_t::array;
    for (int i = 0; i < IChunksManager::kMaxChunkNb; i++) {
        if (chunksToDestroy_[i]) {
            newJson["chunksToDestroy"][nb] = i;
            nb++;
        }
    }

    //Chunks to hide
    nb = 0;
    newJson["chunksToHide"] = nlohmann::detail::value_t::array;
    for (int i = 0; i < IChunksManager::kMaxChunkNb; i++) {
        if (chunksToHide_[i]) {
            newJson["chunksToHide"][nb] = i;
            nb++;
        }
    }
    return newJson;
}

void Chunk::SetFromJson(const json& json, const std::map<ecs::EntityIndex, ecs::EntityIndex>& baseParentIdToOffsetParentId)
{
    //position
    position_.SetFromJson(json["position"]);

    //extent
    extent_.SetFromJson(json["size"]);

    //link Entities
    linkEntities_.resize(json["gameObjects"].size());
    
    for (size_t i = 0; i < linkEntities_.size(); i++) {
		const ecs::EntityIndex baseEntityIndex = json["gameObjects"][i];
        linkEntities_[i] = baseParentIdToOffsetParentId.at(baseEntityIndex);
    }

    //Entities to set active
    cassert(
        CheckJsonExists(json, "entitiesIdToActivate"),
        "The json for the chunk is missing the chunksToActivate variable");
    entitiesToActivate_.resize(json["entitiesIdToActivate"].size());
    for (size_t i = 0; i < entitiesToActivate_.size(); i++) {
		ecs::EntityIndex baseEntityIndex = json["entitiesIdToActivate"][i];
        entitiesToActivate_[i] = baseParentIdToOffsetParentId.at(baseEntityIndex);
    }

    //Chunks to show
    cassert(
        CheckJsonExists(json, "chunksToShow"),
        "The json for the chunk is missing the chunksToShow variable");
    for (const auto& i : json["chunksToShow"]) { chunksToShow_.set(i); }

    //Chunks to destroy
    cassert(
        CheckJsonExists(json, "chunksToDestroy"),
        "The json for the chunk is missing the chunksToDestroy variable");
    for (const auto& i : json["chunksToDestroy"]) { chunksToDestroy_.set(i); }

    //Chunks to hide
    cassert(
        CheckJsonExists(json, "chunksToHide"),
        "The json for the chunk is missing the chunksToHide variable");
    for (const auto& i : json["chunksToHide"]) { chunksToHide_.set(i); }

    std::sort(linkEntities_.begin(), linkEntities_.end());
}

bool Chunk::IsTransformInside(
    const math::Transform& transform) const
{
    const math::Vec3 position = transform.GetLocalPosition();

    const float maxX = position_.x + extent_.x * 0.5f;
    const float minX = position_.x - extent_.x * 0.5f;

    const float maxY = position_.y + extent_.y * 0.5f;
    const float minY = position_.y - extent_.y * 0.5f;

    const float maxZ = position_.z + extent_.z * 0.5f;
    const float minZ = position_.z - extent_.z * 0.5f;

    return
        maxX >= position.x && position.x >= minX &&
        maxY >= position.y && position.y >= minY &&
        maxZ >= position.z && position.z >= minZ;
}

bool Chunk::Overlap(const physics::AABB aabb) const
{
    const physics::AABB chunkAABB{position_, extent_};

    return chunkAABB.OverlapAABB(aabb);
}

bool Chunk::IsEntityInside(
    const ecs::EntityIndex entity,
    const math::Vec3 position) const
{
    if (entity <= linkEntities_.front() || entity >= linkEntities_.back()) { return false; }

    const auto it = std::find(
        linkEntities_.begin(),
        linkEntities_.end(),
        entity);

    if (it == linkEntities_.end()) { return false; }

    return IsPositionInside(position);
}

bool Chunk::IsPositionInside(const math::Vec3 position) const
{
    const float maxX = position_.x + extent_.x * 0.5f;
    const float minX = position_.x - extent_.x * 0.5f;

    const float maxY = position_.y + extent_.y * 0.5f;
    const float minY = position_.y - extent_.y * 0.5f;

    const float maxZ = position_.z + extent_.z * 0.5f;
    const float minZ = position_.z - extent_.z * 0.5f;

    return
        maxX >= position.x && position.x >= minX &&
        maxY >= position.y && position.y >= minY &&
        maxZ >= position.z && position.z >= minZ;
}

void Chunk::AddLinkEntity(const ecs::EntityIndex entity)
{
    const auto it = std::find(linkEntities_.begin(), linkEntities_.end(), entity);

    if (it != linkEntities_.end()) { return; }

    if (linkEntities_.size() == linkEntities_.capacity()) {
        linkEntities_.reserve(linkEntities_.size() * 2);
    }

    linkEntities_.emplace_back(entity);
}

void Chunk::RemoveLinkEntity(const ecs::EntityIndex entityIndex)
{
    const auto it = std::find(
        linkEntities_.begin(),
        linkEntities_.end(),
        entityIndex);

    if (it != linkEntities_.end()) { linkEntities_.erase(it); }
}

void Chunk::AddEntityToActivate(ecs::EntityIndex entity)
{
    const auto it = std::find(entitiesToActivate_.begin(), entitiesToActivate_.end(), entity);

    if (it != entitiesToActivate_.end()) { return; }

    if (entitiesToActivate_.size() == entitiesToActivate_.capacity()) {
        entitiesToActivate_.reserve(entitiesToActivate_.size() * 2 + 1);
    }

    entitiesToActivate_.emplace_back(entity);
}

void Chunk::RemoveEntityToActivate(const ecs::EntityIndex entityIndex)
{
    const auto it = std::find(
        entitiesToActivate_.begin(),
        entitiesToActivate_.end(),
        entityIndex);

    if (it != entitiesToActivate_.end()) { entitiesToActivate_.erase(it); }
}

void Chunk::AddChunkToShow(const ChunkIndex chunkIndex)
{
    cassert(
        chunkIndex < IChunksManager::kMaxChunkNb,
        "the chunk index must be smalle than the kMaxChunkNb");
    chunksToShow_.set(chunkIndex);
}

void Chunk::RemoveChunkToShow(const ChunkIndex chunkIndex)
{
    cassert(
        chunkIndex < IChunksManager::kMaxChunkNb,
        "the chunk index must be smalle than the kMaxChunkNb");
    chunksToShow_.set(chunkIndex, false);
}

void Chunk::AddChunkToHide(const ChunkIndex chunkIndex)
{
    cassert(
        chunkIndex < IChunksManager::kMaxChunkNb,
        "the chunk index must be smalle than the kMaxChunkNb");
    chunksToHide_.set(chunkIndex);
}

void Chunk::RemoveChunkToHide(const ChunkIndex chunkIndex)
{
    cassert(
        chunkIndex < IChunksManager::kMaxChunkNb,
        "the chunk index must be smalle than the kMaxChunkNb");
    chunksToHide_.set(chunkIndex, false);
}

void Chunk::AddChunkToDestroy(const ChunkIndex chunkIndex)
{
    cassert(
        chunkIndex < IChunksManager::kMaxChunkNb,
        "the chunk index must be smalle than the kMaxChunkNb");
    chunksToDestroy_.set(chunkIndex);
}

void Chunk::RemoveChunkToDestroy(const ChunkIndex chunkIndex)
{
    cassert(
        chunkIndex < IChunksManager::kMaxChunkNb,
        "the chunk index must be smalle than the kMaxChunkNb");
    chunksToDestroy_.set(chunkIndex, false);
}

void Chunk::Destroy() { EcsManagerLocator::Get().DestroyEntities(linkEntities_); }

const std::vector<ecs::EntityIndex>& Chunk::GetEntitiesToActivate() const
{
    return entitiesToActivate_;
}

void Chunk::SetEntitiesToActivate(const std::vector<ecs::EntityIndex>& entitiesToActivate)
{
    entitiesToActivate_ = entitiesToActivate;
}

const std::vector<ecs::EntityIndex>& Chunk::GetLinkEntities() const { return linkEntities_; }

void Chunk::SetLinkEntities(const std::vector<ecs::EntityIndex>& linkEntities)
{
    linkEntities_ = linkEntities;
}
} //namespace chunk
} //namespace poke
