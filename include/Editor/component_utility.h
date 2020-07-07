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
// Author : Guillaume Volgyesi
// Co-Author : Duncan Bourquard / Nicolas Schneider
// Date : 20.02.2020
//----------------------------------------------------------------------------------
#pragma once

#include <string>
#include <map>

#include <ImGuizmo.h>

#include <Editor/editor.h>
#include <Editor/Components/editor_component.h>

#include <Game/ComponentManagers/player_manager.h>
#include <Game/ComponentManagers/enemies_manager.h>
#include <Game/ComponentManagers/weapon_manager.h>
#include <Game/ComponentManagers/projectile_manager.h>
#include <Game/ComponentManagers/missiles_manager.h>
#include <Game/ComponentManagers/destructible_element_manager.h>
#include <Editor/Components/editor_component.h>
#include <Game/ComponentManagers/spline_state_manager.h>

namespace poke {

class ICamera;
class Engine;

namespace ecs {
class CoreEcsManager;
} //namespace ecs

namespace editor {

class ComponentUtility
{
public:

	ComponentUtility() = default;

    // Components //
	static const math::Transform DisplayAndModifyTransform(const math::Transform& transform);
	static const physics::Rigidbody DisplayAndModifyRigidbody(const physics::Rigidbody& rigidbody);
	static const physics::Collider DisplayAndModifyCollider(const physics::Collider& collider);
	static const ecs::SplineFollower DisplayAndModifySplineFollower(const ecs::SplineFollower& splineFollower);
	static const graphics::Model DisplayAndModifyModel(const graphics::Model& model);
	static const MaterialSkybox DisplayAndModifyMaterialSkybox(const MaterialSkybox& material);
	static const MaterialDiffuse DisplayAndModifyMaterialDiffuse(const MaterialDiffuse& material);
	static const MaterialTrail DisplayAndModifyMaterialTrail(const MaterialTrail& material);
	static const MaterialParticle DisplayAndModifyMaterialParticle(const MaterialParticle& material);
	static const ecs::Light DisplayAndModifyLight(const ecs::Light& light);
	static const graphics::ParticleSystem DisplayAndModifyParticleSystem(const graphics::ParticleSystem& particleSystem);
	static const ecs::TrailRenderer DisplayAndModifyTrailRenderer(const ecs::TrailRenderer& trailRenderer);
	static const audio::AudioSource DisplayAndModifyAudioSource(const audio::AudioSource& audioSource);
	static const ecs::SegmentRenderer DisplayAndModifySegmentRenderer(const ecs::SegmentRenderer& segmentRenderer);

    // Game components
	static const game::Enemy DisplayAndModifyEnemy(const game::Enemy& enemy);
    static const game::Player DisplayAndModifyPlayer(const game::Player& player);
	static const game::DestructibleElement DisplayAndModifyDestructibleElement(const game::DestructibleElement& destructibleElement);
	static const game::Weapon DisplayAndModifyWeapon(const game::Weapon& weapon, bool treeNode = false);
	static const game::Projectile DisplayAndModifyProjectile(const game::Projectile& projectile, bool treeNode = false);
	static const game::Missile DisplayAndModifyMissile(const game::Missile& missile, bool treeNode = false);
	static const game::SpecialAttack DisplayAndModifySpecialAttack(const game::SpecialAttack& specialAttack);
	static const game::SplineStates DisplayandModifySplineStates(const game::SplineStates& splineStates);
	static const game::Jiggle DisplayAndModifyJiggle(const game::Jiggle& jiggle);
	static const game::GameCamera DisplayAndModifyGameCamera(const game::GameCamera& gameCamera);
    // !Game components

	static const ecs::EditorComponent DisplayAndModifyEditorComponent(const ecs::EditorComponent editorComponent);

	static const void DisplayTransformEditor();
	static const math::Transform DisplayTransformEditorGizmo(
		const math::Transform& transform);

	/**
     * \brief Display remove button of the component
     */
	static void DisplayRemoveComponentButton(
		const ecs::ComponentType::ComponentType componentType);

    static XXH64_hash_t CreateNewMaterial(
		const std::string& newMaterialName,
		const graphics::MaterialType materialType);

	/**
	 * \brief Get a string name of a component
	 * \param componentType : the component that you want
	 * \param inJson : true if the name is used in a Json
	 * \return a string with the component name
	 */
	static std::string GetComponentName(
		ecs::ComponentType::ComponentType componentType,
		bool inJson = false);

	static void SetEditor(Editor& editor) {
	    editor_.emplace(editor);
	}
private:

	inline static std::experimental::optional<Editor&> editor_ = std::experimental::nullopt;

	const inline static float kDragSpeed = 0.1f;

	//ImGuizmo
	inline static bool gridVisible_ = false;
	inline static float gridSize_ = 10.0f;
	inline static bool cubeVisible_ = false;
	inline static bool useSnap_ = false;
	inline static float snap_[3] = { 1.f, 1.f, 1.f };
	inline static float bounds_[6] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	inline static float boundsSnap_[3] = { 1.0f, 1.0f, 1.0f };
	inline static bool boundSizing_ = false;
	inline static ImGuizmo::MODE mCurrentGizmoMode_ = ImGuizmo::LOCAL;
	inline static ImGuizmo::OPERATION mCurrentGizmoOperation_ = ImGuizmo::TRANSLATE;

    //Model Display
	inline static constexpr bool kAlphaPreview = true;
	inline static constexpr bool kAlphaHalfPreview = false;
	inline static constexpr bool kDragAndDrop = true;
	inline static constexpr bool kOptionsMenu = true;
	inline static constexpr bool kHdr = false;

	inline static bool openNewMaterialPopup_ = false;
	inline static bool setWithNew_ = false;
	inline static std::string newName_ = "";
	inline static graphics::MaterialType newMaterialType_ = graphics::MaterialType::DIFFUSE;
    
	inline static std::vector<std::string> comboBoxElements_ = std::vector<std::string>();
};
}//namespace editor
}//namespace poke

