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
// Date : 17.02.20
//----------------------------------------------------------------------------------
#pragma once

#include <vector>

#include <Ecs/ComponentManagers/interface_components_manager.h>
#include <Math/tranform.h>
#include <Memory/vector_view.h>

namespace poke::ecs {

class TransformsManager : public IComponentsManager {
public:
    TransformsManager() = default;
    ~TransformsManager() = default;

    /**
     * \brief Add a lot of entity in one go.
     * \param size : the number of entity that you want to add.
     */
    void ResizeEntities(size_t size) override;

    /**
     * \brief Remove all component from an entity and set it inactive
     * \param entityIndex : the index of the entity
     */
    void ClearEntity(EntityIndex entityIndex) override;

    void SetWithArchetype(EntityPool entityPool, const Archetype& archetype) override;
    void InsertArchetype(EntityIndex entity, const Archetype& archetype) override;
    void EraseEntities(EntityPool pool, size_t nbObjectToErase) override;

    /**
     * \brief Get a struct with all the data from the component of one entity
     * \param entityIndex : the index of the entity
     * \return a struct with all the data and the index of the component
     */
    const math::Transform& GetComponent(EntityIndex entityIndex) const;

    void SetComponent(EntityIndex entityIndex, const math::Transform& transform);
    void SetComponentFromWorldTransform(EntityIndex entityIndex, const math::Transform& worldTransform);

    /**
	 * \brief Get the parent of the given entity
	 * \param entityIndex 
	 * \return 
	 */
    EntityIndex GetParent(EntityIndex entityIndex);

    /**
	 * \brief Set the parent of the entity
	 * \param entityIndex 
	 * \param parent 
	 */
    void SetParent(EntityIndex entityIndex, EntityIndex parent);

    /**
	 * \brief Return all parents
	 * \return 
	 */
    std::vector<EntityIndex>& GetParents();

    /**
     * \brief Get children of the given entity.
     * \param entityIndex 
     * \return 
     */
    std::vector<EntityIndex>& GetChildren(EntityIndex entityIndex);

    /**
     * \brief Get the matrix to pass from local space to world space
     * \param entityIndex 
     * \return 
     */
    math::Matrix4 GetLocalToWorldMatrix(EntityIndex entityIndex);

    /**
     * \brief Get the matrix to pass from world space to local space
     * \param entityIndex 
     * \return 
     */
    math::Matrix4 GetWorldToLocalMatrix(EntityIndex entityIndex);

    /**
     * \brief Get the world position of the given index
     * \param entityIndex 
     * \return 
     */
    math::Vec3 GetWorldPosition(EntityIndex entityIndex);
    /**
     * \brief Get the world scale of the given index
     * \param entityIndex
     * \return
     */
    math::Vec3 GetWorldScale(EntityIndex entityIndex);
    /**
     * \brief Get the world rotation of the given index
     * \param entityIndex
     * \return
     */
    math::Vec3 GetWorldRotation(EntityIndex entityIndex);

    /**
     * \brief Get the world transform of the given index
     * \param entityIndex
     * \return
     */
    math::Transform GetWorldTransform(EntityIndex entityIndex);

    /**
     * \brief Return the world position of the given point from the entity world space
     * \param entityIndex 
     * \param point 
     * \return 
     */
    math::Vec3 InverseTransformPoint(EntityIndex entityIndex, math::Vec3 point);

    void SetComponentFromJson(
        EntityIndex entityIndex,
        const json& componentJson) override;

    json GetJsonFromComponent(EntityIndex entityIndex) override;

	constexpr static int GetComponentIndex()
	{
		return math::log2(static_cast<int>(ComponentType::TRANSFORM));
	}

    /**
     * \brief Return a vector view of all transform. WARNING if the number of entity get bigger then it won't be the correct vector view anymore.
     * \return 
     */
    VectorView<std::vector<math::Transform>::iterator> GetTransformsView();

	/**
	 * \brief Return a vector view of the given entityPool. This function is meant to work with archetypes. WARNING if the number of entity get bigger then it won't be the correct vector view anymore.
	 * \return
	 */
    VectorView<std::vector<math::Transform>::iterator> GetTransformsView(EntityPool entityPool);
private:
    void SetDirty(EntityIndex entityIndex);

    math::Matrix4 CalculateLocalToParentMatrix(EntityIndex entityIndex) const;

    math::Vec3 GetLocalRotationFromWorldRotation(
        EntityIndex entityIndex,
        math::Vec3 rotation
    ) const;
    math::Vec3 GetLocalScaleFromWorldScale(
        EntityIndex entityIndex,
        math::Vec3 scale
    ) const;

    std::vector<math::Transform> transforms_;

    std::vector<EntityIndex> parents_;
    std::vector<std::vector<EntityIndex>> children_;

    std::vector<math::TransformDirtyFlag> dirtyFlags_;

    std::vector<math::Matrix4> worldToLocalMatrices_;
    std::vector<math::Matrix4> localToWorldMatrices_;
};
} //namespace poke::ecs
