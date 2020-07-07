#include <Ecs/Components/spline_follower.h>

namespace poke {
namespace ecs {

SplineFollower::SplineFollower(
    std::vector<math::Vec3>& pointsPositions)
    : spline(pointsPositions) { }

bool SplineFollower::operator==(const SplineFollower& other) const
{
    return spline == other.spline
           && lastPoint == other.lastPoint
           && segmentPercentage == other.segmentPercentage
           && speed == other.speed;
}

bool SplineFollower::operator!=(const SplineFollower& other) const
{
    return !(*this == other);
}

json SplineFollower::ToJson() const
{
    json splineJson;
    auto points = spline.GetPoints();
	splineJson["points"] = json::value_t::array;
	for (size_t i = 0; i < points.size(); i++) {
        splineJson["points"][i] = points[i].ToJson();
    }
    return splineJson;
}

void SplineFollower::SetFromJson(const json& splineJson)
{
	for (int i = 0; i < splineJson["points"].size(); i++) {
		math::Vec3 pos;
		pos.SetFromJson(splineJson["points"][i]);
		spline.AddPoint(pos);
	}
}
} //namespace ecs
} //namespace poke
