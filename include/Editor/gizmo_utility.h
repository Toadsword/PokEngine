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
// Author : Duncan Bourquard
// Co-Author : 
// Date : 31.03.2020
//----------------------------------------------------------------------------------
#pragma once

#include <Editor/editor.h>

#include <utility>



namespace poke {
namespace editor {

enum GizmoDisplayMode : uint8_t {
    ALL_ENTITIES,
    SELECTED_ENTITY_ONLY,
    NONE
};

enum GizmoFlagEnum : uint16_t {
	GIZMO_FLAG_NONE             = 0,
	GIZMO_FLAG_CHUNK            = 1 << 0,
	GIZMO_FLAG_SPLINE           = 1 << 1,
	GIZMO_FLAG_SPLINE_POINTS    = 1 << 2,
	GIZMO_FLAG_COLLIDER         = 1 << 3,
	GIZMO_FLAG_AABB             = 1 << 4,
	GIZMO_FLAG_DIRECTION        = 1 << 5,
	GIZMO_FLAG_TRANSFORM        = 1 << 6,

	GIZMO_FLAG_ALL              = (1 << 7) - 1,
};

using GizmoFlags = int;

class GizmoUtility
{
public:

	GizmoUtility() = default;

    // Components //
	static const void DisplayAabbGizmo(
		const physics::AABB& aabb,
		Color color = gizmoColorAabb_);

	static const void DisplayAllChunksGizmo();
	static const void DisplayChunkGizmo(uint8_t chunkIndex);

	static const void DisplayTransformGizmo(
		const math::Transform& transform,
		Color color = gizmoColorTransform_);

    static const void DisplayColliderGizmo(
		const physics::Collider& collider,
		const math::Transform& transform,
		Color color = gizmoColorCollider_);

    static const void DisplayDirectionGizmo(
		const math::Vec3& origin,
		const math::Vec3& direction, 
		Color color = gizmoColorDirection_);

    static const void DisplaySplineGizmo(
		const math::CubicHermiteSpline& spline, 
		Color color = gizmoColorSpline_);

	static const void DisplaySplineGizmo(
		const ecs::SplineFollower& splineFollower,
		Color splineColor = gizmoColorSpline_,
		Color followerColor = gizmoColorSplineFollower_);

	static const void DisplaySplinePointsGizmo(
		const game::SplineStates& splineStates,
		const ecs::SplineFollower& splineFollower,
		Color splineStatesColor = gizmoColorSplineStates_,
		Color directionColor = gizmoColorDirection_);

	static void DisplayGizmos();

	static void DisplayGizmoMenu(const char * label);

	static GizmoFlags DisplayCheckboxEntity(GizmoFlags flags, GizmoFlagEnum flagToModify);

	static const char * GetGizmoName(GizmoFlagEnum gizmoFlag);

	static void SetEditor(Editor& editor) { editor_.emplace(editor); }

	static GizmoFlags GetGizmoFlags() {
		return gizmoFlags_;
	}
	static void SetGizmoFlags(GizmoFlags newGizmoFlags) {
		gizmoFlags_ = newGizmoFlags;
	}

	static std::vector<int> GetChunkIndexsNoDisplayGizmo() {
		return chunkIndexsNoDisplayGizmo_;
	}
	static void SetChunkIndexsNoDisplayGizmo(std::vector<int>& newList) {
		chunkIndexsNoDisplayGizmo_ = newList;
	}

    static float GetTransformPointRadius() { return transformPointRadius_; }
    static void SetTransformPointRadius(float newValue) { transformPointRadius_ = newValue; }

	static float GetDirectionLineLength() { return directionLineLength_; }
	static void SetDirectionLineLength(float newValue) { directionLineLength_ = newValue; }

    static float GetEndArrowSphereRadius() { return endArrowSphereRadius_; }
	static void SetEndArrowSphereRadius(float newValue) { endArrowSphereRadius_ = newValue; }

	static float GetIterationSpace() { return iterationSpace_; }
	static void SetIterationSpace(float newValue) { iterationSpace_ = newValue; }

	static Color GetChunkGizmoColor(chunk::ChunkStatus chunkStatus);
	static void SetChunkGizmoColor(chunk::ChunkStatus chunkStatus, Color newColor);

	static Color GetGizmoColor(GizmoFlagEnum gizmoFlag);
	static void SetGizmoColor(GizmoFlagEnum gizmoFlag, Color newColor);

private:
	static bool DisplaySubMenuGizmoItem(GizmoFlagEnum gizmoFlag);

	inline static std::vector<int> chunkIndexsNoDisplayGizmo_;

	inline static GizmoFlags gizmoFlags_ = ALL_ENTITIES;

	inline static GizmoDisplayMode gizmoDisplayMode_ = ALL_ENTITIES;

	inline static std::experimental::optional<Editor&> editor_ = std::experimental::nullopt;

	// Options for Transform
	inline static float transformPointRadius_ = 0.5f;

	// Options for Direction
	inline static float directionLineLength_ = 2.0f;
	inline static float endArrowSphereRadius_ = 0.5f;

	// Options for spline
	inline static float iterationSpace_ = 0.1f;

	inline static Color gizmoColorChunkVisible_ = Color::aqua;
	inline static Color gizmoColorChunkHidden_ = Color::grey;
	inline static Color gizmoColorChunkActive_ = Color::blue;
	inline static Color gizmoColorChunkDestroyed_ = Color::black;

	inline static Color gizmoColorSpline_ = Color::red;
	inline static Color gizmoColorSplineStates_ = Color::orange;
	inline static Color gizmoColorSplineFollower_ = Color::blue;
	inline static Color gizmoColorCollider_ = Color::green;
	inline static Color gizmoColorAabb_ = Color::red;
	inline static Color gizmoColorDirection_ = Color::blue;
	inline static Color gizmoColorTransform_ = Color::purple;

};
}//namespace editor
}//namespace poke

