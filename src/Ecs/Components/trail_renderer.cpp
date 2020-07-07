#include <Ecs/Components/trail_renderer.h>
#include <GraphicsEngine/Models/mesh.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace ecs {
bool TrailRenderer::operator==(const TrailRenderer& other) const
{
    return materialID == other.materialID
           && lifetime == other.lifetime
           && changeTime == other.changeTime
           && widthStart == other.widthStart
           && widthEnd == other.widthEnd
           && vertexDistanceMin == other.vertexDistanceMin
           && renderDirection == other.renderDirection
           && isPaused == other.isPaused
           && centerPositions_ == other.centerPositions_
           && leftVertices_ == other.leftVertices_
           && rightVertices_ == other.rightVertices_;
}

bool TrailRenderer::operator!=(const TrailRenderer& other) const { return !(*this == other); }

bool TrailRenderer::Update(const math::Vec3 worldPos)
{
	if (centerPositions_.empty()) {
		centerPositions_.push_back(worldPos);
		return false;
	}

    const float dt = Time::Get().deltaTime.count() / 1000.0f;

    for (auto& leftVertex : leftVertices_) { leftVertex.timeAlive += dt; }

    for (auto& rightVertex : rightVertices_) { rightVertex.timeAlive += dt; }

    //set the mesh and adjust widths if vertices were added or removed
    if (TryAddVertices(worldPos) | TryRemoveVertices() | SetVertexWidth()) {
		return true;
    }
	return false;
}

size_t TrailRenderer::GetNumberOfPoints() const { return centerPositions_.size(); }

void TrailRenderer::Clear()
{
    centerPositions_.clear();
    centerPositions_.shrink_to_fit();
    leftVertices_.clear();
    leftVertices_.shrink_to_fit();
    rightVertices_.clear();
    rightVertices_.shrink_to_fit();
}

void TrailRenderer::SetFromJson(const json& trailRendererJson)
{
    //Data fields
    materialID = trailRendererJson["materialID"];
    lifetime = trailRendererJson["lifetime"];
    changeTime = trailRendererJson["changeTime"];
    widthStart = trailRendererJson["widthStart"];
    widthEnd = trailRendererJson["widthEnd"];
    vertexDistanceMin = trailRendererJson["vertexDistanceMin"];
    renderDirection.SetFromJson(trailRendererJson["renderDirection"]);
    isPaused = trailRendererJson["isPaused"];
}

json TrailRenderer::ToJson() const
{
    json trailRendererJson;

    //Data fields
    trailRendererJson["materialID"] = materialID;
    trailRendererJson["lifetime"] = lifetime;
    trailRendererJson["changeTime"] = changeTime;
    trailRendererJson["widthStart"] = widthStart;
    trailRendererJson["widthEnd"] = widthEnd;
    trailRendererJson["vertexDistanceMin"] = vertexDistanceMin;
    trailRendererJson["renderDirection"] = renderDirection.ToJson();
    trailRendererJson["isPaused"] = isPaused;

    return trailRendererJson;
}

bool TrailRenderer::TryAddVertices(const math::Vec3 worldPos)
{
	const auto cameraFront = CameraLocator::Get().GetFront();

    if (math::Vec3::GetDistanceManhattan(centerPositions_.back(), worldPos) > vertexDistanceMin * vertexDistanceMin) {
        //Calculate the normalized direction from the ) most recent position of vertex creation to the 2) current position
        const math::Vec3 dirToCurrentPos = (worldPos - centerPositions_.back()).Normalize();

        //Calculate the positions of the left and right vertices -> they are perpendicular to 'dirToCurrentPos' and 'renderDirection'
        const math::Vec3 cross = math::Vec3::Cross(cameraFront, dirToCurrentPos);
        const math::Vec3 leftPos = worldPos + cross * -widthStart * 0.5f;
        const math::Vec3 rightPos = worldPos + cross * widthStart * 0.5f;

        //Calculate left and right pos
        leftVertices_.emplace_back(leftPos, worldPos, (leftPos - worldPos).Normalize());
        rightVertices_.emplace_back(rightPos, worldPos, (rightPos - worldPos).Normalize());

        //Add new position
        centerPositions_.push_back(worldPos);
        return true;
    }

    return false;
}

bool TrailRenderer::TryRemoveVertices()
{
	bool vertexRemoved = false;

    while(!leftVertices_.empty() && leftVertices_.front().timeAlive > lifetime) {
		leftVertices_.erase(leftVertices_.begin());
		rightVertices_.erase(rightVertices_.begin());
		centerPositions_.erase(centerPositions_.begin());

		vertexRemoved = true;
    }
    return vertexRemoved;
}

bool TrailRenderer::SetVertexWidth()
{
    if (widthStart == widthEnd) { return false; }

    const float widthDelta = widthStart - widthEnd;
    const float timeDelta = lifetime - changeTime;

    for (size_t i = 0; i < leftVertices_.size(); i++) {
        if (leftVertices_[i].timeAlive > changeTime) {
            const float width = widthStart - (widthDelta * ((leftVertices_[i].timeAlive - changeTime) / (timeDelta + 0.00001f)));

            const float halfWidth = width * 0.5f;

            leftVertices_[i].AdjustWidth(halfWidth);
            rightVertices_[i].AdjustWidth(halfWidth);
        }
    }

    return true;
}

std::pair<std::vector<graphics::VertexMesh>, std::vector<uint32_t>> TrailRenderer::GenerateMesh()
{
	//Only continue if there are at least two center positions in the collection
	if (leftVertices_.size() < 2) {
		return {};
	}

	std::vector<graphics::VertexMesh> vertices(leftVertices_.size() * 2);
	std::vector<uint32_t> indexes((leftVertices_.size() - 1) * 6);

	//Get the change in time between the first and last pair of vertices.
	const float timeDelta = leftVertices_.back().timeAlive - leftVertices_.front().timeAlive + 0.0001f;

	//Iterate through all the pairs of vertices (left + right).
	for (size_t i = 0, j = 0; i < leftVertices_.size(); i++, j += 2) {
		const auto leftVertex = leftVertices_[i];
		const auto rightVertex = rightVertices_[i];

		const float uvValue = leftVertex.timeAlive / timeDelta;

		vertices[j] = graphics::VertexMesh(
			leftVertex.position,
			math::Vec2(uvValue, 0),
			{0, 0, 0}
			);

		vertices[j + 1] = graphics::VertexMesh(
			rightVertex.position,
			math::Vec2(uvValue, 1),
			{0, 0, 0}
		);

		//trail position
		const int vertexIndex = i * 2;

		//trail triangles
		if (i > 0) {
			const int triIndex = (i - 1) * 6;
			indexes[triIndex] = vertexIndex - 2;
			indexes[triIndex + 1] = vertexIndex - 1;
			indexes[triIndex + 2] = vertexIndex + 1;
			indexes[triIndex + 3] = vertexIndex - 2;
			indexes[triIndex + 4] = vertexIndex + 1;
			indexes[triIndex + 5] = vertexIndex;
		}
	}

	return {vertices, indexes};
}
} //namespace ecs
} //namespace poke
