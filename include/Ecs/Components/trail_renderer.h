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
// Date : 28.03.2020
//----------------------------------------------------------------------------------
#pragma once

#include <Utility/json_utility.h>
#include <ResourcesManager/resource_type.h>
#include <Math/vector.h>
#include <Utility/time_custom.h>
#include <GraphicsEngine/Models/mesh.h>

namespace poke {
namespace ecs {
struct TrailRendererVertex {
    bool operator==(const TrailRendererVertex& other) const
    {
        return centerPosition == other.centerPosition
               && derivedDirection == other.derivedDirection
               && position == other.position
               && creationTime == other.creationTime
               && timeAlive == other.timeAlive;
    }

    bool operator!=(const TrailRendererVertex& other) const
    {
        return !(*this == other);
    }

	TrailRendererVertex(const math::Vec3 position, const math::Vec3 centerPosition, const math::Vec3 derivedDirection) :
		centerPosition(centerPosition),
		derivedDirection(derivedDirection),
		position(position),
		timeAlive(0)
	{
		creationTime = Time::Get().GetTime();
	}

	void AdjustWidth(const float width) {
		position = centerPosition + (derivedDirection * width);
	}

	math::Vec3 centerPosition; //the center position in the trail that this vertex was derived from
	math::Vec3 derivedDirection; //the direction from the 1) center position to the 2) position of this vertex
	math::Vec3 position;
	float creationTime;
	float timeAlive;
};

struct TrailRenderer {
	TrailRenderer() = default;

	~TrailRenderer() = default;

    bool operator==(const TrailRenderer& other) const;

    bool operator!=(const TrailRenderer& other) const;

    bool Update(math::Vec3 worldPos);

    size_t GetNumberOfPoints() const;

	const std::vector<math::Vec3>& GetCenterPosition() const { return centerPositions_; }
	const std::vector<TrailRendererVertex>& GetLeftVertices() const { return leftVertices_; }
	const std::vector<TrailRendererVertex>& GetRightVertices() const { return rightVertices_; }

    void Clear();

    void SetFromJson(const json& trailRendererJson);

    json ToJson() const;

    //Data fields
	ResourceID materialID = math::HashString("default_trail");
	float lifetime = 1.0f;
	float changeTime = 0.5f;
	float widthStart = 1.0f;
	float widthEnd = 1.0f;
	float vertexDistanceMin = 0.1f;
	math::Vec3 renderDirection = {0, 1, 0};
	bool isPaused = false;

	std::pair<std::vector<graphics::VertexMesh>, std::vector<uint32_t>> GenerateMesh();
private:
    bool TryAddVertices(math::Vec3 worldPos);

    bool TryRemoveVertices();

    bool SetVertexWidth();


    //Logic fields
	std::vector<math::Vec3> centerPositions_;
	std::vector<TrailRendererVertex> leftVertices_;
	std::vector<TrailRendererVertex> rightVertices_;
};
} //namespace ecs
} //namespace poke
