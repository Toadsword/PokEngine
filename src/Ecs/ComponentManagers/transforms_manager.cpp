#include <Ecs/ComponentManagers/transforms_manager.h>
#include <Utility/profiler.h>

namespace poke::ecs {
void TransformsManager::ResizeEntities(const size_t size)
{
    transforms_.resize(size);
    parents_.resize(size, kNoParent);
    dirtyFlags_.resize(
        size,
        math::TransformDirtyFlagStatus::IS_LOCAL_DIRTY | math::
        TransformDirtyFlagStatus::IS_WORLD_DIRTY);
    worldToLocalMatrices_.resize(size);
    localToWorldMatrices_.resize(size);
    children_.resize(size);
}

void TransformsManager::ClearEntity(const EntityIndex entityIndex)
{
    dirtyFlags_[entityIndex] = math::TransformDirtyFlagStatus::IS_LOCAL_DIRTY | math::
                               TransformDirtyFlagStatus::IS_WORLD_DIRTY;
    transforms_[entityIndex] = math::Transform();
    parents_[entityIndex] = kNoParent;
    children_[entityIndex].clear();
}

void TransformsManager::SetWithArchetype(
    const EntityPool entityPool,
    const Archetype& archetype)
{
    for (EntityIndex entityIndex = entityPool.firstEntity; entityIndex < entityPool.lastEntity;
         entityIndex++) {
        transforms_[entityIndex] = archetype.transform;
        parents_[entityIndex] = kNoParent;
    }
}

void TransformsManager::InsertArchetype(
    const EntityIndex entity,
    const Archetype& archetype)
{
    transforms_.insert(transforms_.begin() + entity, archetype.transform);
    worldToLocalMatrices_.insert(worldToLocalMatrices_.begin() + entity, math::Matrix4::Identity());
    localToWorldMatrices_.insert(localToWorldMatrices_.begin() + entity, math::Matrix4::Identity());

    dirtyFlags_.insert(
        dirtyFlags_.begin() + entity,
        math::TransformDirtyFlagStatus::IS_LOCAL_DIRTY | math::TransformDirtyFlagStatus::
        IS_WORLD_DIRTY);

    parents_.insert(parents_.begin() + entity, kNoParent);
    children_.insert(children_.begin() + entity, std::vector<EntityIndex>());
}

void TransformsManager::EraseEntities(const EntityPool pool, const size_t nbObjectToErase)
{
    transforms_.erase(
        transforms_.begin() + pool.firstEntity,
        transforms_.begin() + pool.firstEntity + nbObjectToErase);

    worldToLocalMatrices_.erase(
        worldToLocalMatrices_.begin() + pool.firstEntity,
        worldToLocalMatrices_.begin() + pool.firstEntity + nbObjectToErase);

    localToWorldMatrices_.erase(
        localToWorldMatrices_.begin() + pool.firstEntity,
        localToWorldMatrices_.begin() + pool.firstEntity + nbObjectToErase);

    dirtyFlags_.erase(
        dirtyFlags_.begin() + pool.firstEntity,
        dirtyFlags_.begin() + pool.firstEntity + nbObjectToErase);

    parents_.erase(
        parents_.begin() + pool.firstEntity,
        parents_.begin() + pool.firstEntity + nbObjectToErase);

    children_.erase(
        children_.begin() + pool.firstEntity,
        children_.begin() + pool.firstEntity + nbObjectToErase);
}

const math::Transform& TransformsManager::GetComponent(const EntityIndex entityIndex) const
{
    return transforms_[entityIndex];
}

void TransformsManager::SetComponent(
    const EntityIndex entityIndex,
    const math::Transform& transform)
{
    transforms_[entityIndex] = transform;
    SetDirty(entityIndex);
}

void TransformsManager::SetComponentFromWorldTransform(
    const EntityIndex entityIndex,
    const math::Transform& worldTransform)
{
    //If the entity has no parent, the worldTransform == localTransform
    if (parents_[entityIndex] == kNoParent) {
        SetComponent(entityIndex, worldTransform);
    } else {
        const auto p = worldTransform.GetLocalPosition();
        //const auto localPos = (GetWorldToLocalMatrix(parents_[entityIndex]) * math::Vec4(p.x, p.y, p.z, 1)).To3();
        //const auto localPos = InverseTransformPoint(parents_[entityIndex], p);
        const auto localPos = (math::Matrix4::GetInverse(GetLocalToWorldMatrix(parents_[entityIndex])) * math::Vec4(p.x, p.y, p.z, 1)).To3();

        //Works fine
        const auto localScale = GetLocalScaleFromWorldScale(
            entityIndex,
            worldTransform.GetLocalScale());

        //Works fine
        const auto localRotation = GetLocalRotationFromWorldRotation(
            entityIndex,
            worldTransform.GetLocalRotation());

        SetComponent(entityIndex, math::Transform(localPos, localRotation, localScale));
    }
}

math::Vec3 TransformsManager::GetLocalRotationFromWorldRotation(
    const EntityIndex entityIndex,
    const math::Vec3 rotation) const
{
    if (parents_[entityIndex] == kNoParent) { return rotation; }

    const auto rot = rotation - transforms_[parents_[entityIndex]].GetLocalRotation();
    return GetLocalRotationFromWorldRotation(parents_[entityIndex], rot);
}

math::Vec3 TransformsManager::GetLocalScaleFromWorldScale(
    const EntityIndex entityIndex,
    const math::Vec3 scale) const
{
    if (parents_[entityIndex] == kNoParent) { return scale; }

    const auto newScale = math::Vec3::Divide(
        scale,
        transforms_[parents_[entityIndex]].GetLocalScale());
    return GetLocalScaleFromWorldScale(parents_[entityIndex], newScale);
}

EntityIndex TransformsManager::GetParent(const EntityIndex entityIndex)
{
    return parents_[entityIndex];
}

void TransformsManager::SetParent(
    const EntityIndex entityIndex,
    const EntityIndex parent)
{
    //If it was a children, clear the children array of the parents
    if (parents_[entityIndex] != kNoParent) {
        const EntityIndex previousParent = parents_[entityIndex];
        const auto it = std::find(
            children_[previousParent].begin(),
            children_[previousParent].end(),
            entityIndex);
        if (it != children_[previousParent].end()) { children_[previousParent].erase(it); }
    }

    parents_[entityIndex] = parent;

    if (parent != kNoParent) { children_[parent].push_back(entityIndex); }
}

std::vector<EntityIndex>& TransformsManager::GetParents() { return parents_; }

std::vector<EntityIndex>& TransformsManager::GetChildren(
    const EntityIndex entityIndex) { return children_[entityIndex]; }

math::Matrix4 TransformsManager::GetLocalToWorldMatrix(const EntityIndex entityIndex)
{
    if ((dirtyFlags_[entityIndex] & math::TransformDirtyFlagStatus::IS_LOCAL_DIRTY) ==
        math::TransformDirtyFlagStatus::IS_LOCAL_DIRTY) {
        if (parents_[entityIndex] == kNoParent) {
            localToWorldMatrices_[entityIndex] = CalculateLocalToParentMatrix(entityIndex);
        } else {
            localToWorldMatrices_[entityIndex] =
                GetLocalToWorldMatrix(parents_[entityIndex]) * CalculateLocalToParentMatrix(entityIndex);
        }
        dirtyFlags_[entityIndex] &= ~math::TransformDirtyFlagStatus::IS_LOCAL_DIRTY;
    }
    return localToWorldMatrices_[entityIndex];
}

math::Matrix4 TransformsManager::GetWorldToLocalMatrix(const EntityIndex entityIndex)
{
    if ((dirtyFlags_[entityIndex] & math::TransformDirtyFlagStatus::IS_WORLD_DIRTY) == math::TransformDirtyFlagStatus::IS_WORLD_DIRTY) {
        worldToLocalMatrices_[entityIndex] = math::Matrix4::GetInverse(GetLocalToWorldMatrix(entityIndex));

        dirtyFlags_[entityIndex] &= ~math::TransformDirtyFlagStatus::
            IS_WORLD_DIRTY;
    }
    return worldToLocalMatrices_[entityIndex];
}

math::Vec3 TransformsManager::GetWorldPosition(const EntityIndex entityIndex)
{
    if (parents_[entityIndex] == kNoParent) { return transforms_[entityIndex].GetLocalPosition(); }

    return (GetLocalToWorldMatrix(parents_[entityIndex]) *
            math::Vec4{
                transforms_[entityIndex].GetLocalPosition().x,
                transforms_[entityIndex].GetLocalPosition().y,
                transforms_[entityIndex].GetLocalPosition().z,
                1
            }).To3();
}

math::Vec3 TransformsManager::GetWorldScale(const EntityIndex entityIndex)
{
    if (parents_[entityIndex] == kNoParent) { return transforms_[entityIndex].GetLocalScale(); }
    return math::Vec3::Multiply(
        transforms_[entityIndex].GetLocalScale(),
        GetWorldScale(parents_[entityIndex]));
}

math::Vec3 TransformsManager::GetWorldRotation(const EntityIndex entityIndex)
{
    if (parents_[entityIndex] == kNoParent) { return transforms_[entityIndex].GetLocalRotation(); }
    return transforms_[entityIndex].GetLocalRotation() + GetWorldRotation(parents_[entityIndex]);
}

math::Transform TransformsManager::GetWorldTransform(const EntityIndex entityIndex)
{
    if (parents_[entityIndex] == kNoParent) { return transforms_[entityIndex]; } else {
        return math::Transform(
            GetWorldPosition(entityIndex),
            GetWorldRotation(entityIndex),
            GetWorldScale(entityIndex));
    }
}

math::Vec3 TransformsManager::InverseTransformPoint(
    const EntityIndex entityIndex,
    const math::Vec3 point)
{
    const auto localMatrix = GetWorldToLocalMatrix(entityIndex);
    const auto res = localMatrix * math::Vec4(point.x, point.y, point.z, 0);
    return math::Vec3(
        res.x,
        res.y,
        res.z);
}

void TransformsManager::SetComponentFromJson(
    const EntityIndex entityIndex,
    const json& componentJson)
{
    transforms_[entityIndex].SetFromJson(componentJson);
	parents_[entityIndex] = componentJson["parent"];

    for (auto& i : children_) {
        const auto it = std::find(i.begin(), i.end(), entityIndex);

        if (it != i.end()) { i.erase(it); }
    }

    if (parents_[entityIndex] != kNoParent) {
        children_[parents_[entityIndex]].push_back(entityIndex);
    }

	SetDirty(entityIndex);
}

json TransformsManager::GetJsonFromComponent(const EntityIndex entityIndex)
{
    json componentJson = transforms_[entityIndex].ToJson();
    componentJson["parent"] = parents_[entityIndex];
    return componentJson;
}

VectorView<std::vector<math::Transform>::iterator> TransformsManager::GetTransformsView()
{
    return VectorView<std::vector<math::Transform>::iterator>(
        transforms_.begin(),
        transforms_.end());
}

VectorView<std::vector<math::Transform>::iterator> TransformsManager::GetTransformsView(
    const EntityPool entityPool)
{
	return VectorView<std::vector<math::Transform>::iterator>(
        transforms_.begin() + entityPool.firstEntity,
		transforms_.end() + entityPool.lastEntity
		);
}

void TransformsManager::SetDirty(const EntityIndex entityIndex)
{
    if ((dirtyFlags_[entityIndex] & math::TransformDirtyFlagStatus::IS_LOCAL_DIRTY) !=
        math::TransformDirtyFlagStatus::IS_LOCAL_DIRTY) {
        dirtyFlags_[entityIndex] |=
            math::TransformDirtyFlagStatus::IS_LOCAL_DIRTY |
            math::TransformDirtyFlagStatus::IS_WORLD_DIRTY;

        //Update children
        for (auto child : children_[entityIndex]) { SetDirty(child); }
    }
}

math::Matrix4 TransformsManager::CalculateLocalToParentMatrix(
    const EntityIndex entityIndex) const
{
    const auto transform = transforms_[entityIndex];

    auto parentMatrix = math::Matrix4::Identity();
    parentMatrix = math::Matrix4::Translate(
        parentMatrix,
        transform.GetLocalPosition());

    const auto localRotation = transform.GetLocalRotation();

    parentMatrix = math::Matrix4::Rotate(
        parentMatrix,
        localRotation.x,
        math::Vec3(1, 0, 0));

    parentMatrix = math::Matrix4::Rotate(
        parentMatrix,
        localRotation.y,
        math::Vec3(0, 1, 0));

    parentMatrix = math::Matrix4::Rotate(
        parentMatrix,
        localRotation.z,
        math::Vec3(0, 0, 1));

    parentMatrix = math::Matrix4::Scale(
        parentMatrix,
        transform.GetLocalScale());

    return parentMatrix;
}
} // namespace poke::ecs
