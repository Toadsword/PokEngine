#include <Game/Components/spline_states.h>

namespace poke {
namespace game {

bool PointState::operator==(const PointState& pointState)const {
	return rotation == pointState.rotation &&
		moveSpeed == pointState.moveSpeed &&
		lerpSpeed == pointState.lerpSpeed &&
		pointIndex == pointState.pointIndex;
}

bool PointState::operator!=(const PointState& pointState)const {
	return !(*this == pointState);
}

const json PointState::ToJson() const {
	json pointStateJson = json::object();
	pointStateJson["rotation"] = rotation.ToJson();
	pointStateJson["moveSpeed"] = moveSpeed;
	pointStateJson["lerpSpeed"] = lerpSpeed;
	pointStateJson["pointIndex"] = pointIndex;
	return pointStateJson;
}
void PointState::SetFromJson(const json& pointStateJson) {
	rotation.SetFromJson(pointStateJson["rotation"]);
	moveSpeed = pointStateJson["moveSpeed"];
	lerpSpeed = pointStateJson["lerpSpeed"];
	pointIndex = pointStateJson["pointIndex"];
}

bool SplineStates::operator==(const SplineStates& splineStates)const {
    if(pointStates.size() != splineStates.pointStates.size()) {
		return false;
    }
	for (size_t i = 0; i < pointStates.size(); i++) {
        if(pointStates[i] != splineStates.pointStates[i]) {
			return false;
        }
    }
	return true;
}
bool SplineStates::operator!=(const SplineStates& splineStates)const {
	return !(*this == splineStates);
}

const json SplineStates::ToJson() const {
	json pointStateJson = json::object();

	pointStateJson["pointStates"] = json::value_t::array;
	for (size_t i = 0; i < pointStates.size(); i++) {
		pointStateJson["pointStates"][i] = pointStates[i].ToJson();
	}
	return pointStateJson;
}

void SplineStates::SetFromJson(const json& splineStatesJson) {

	pointStates.resize(splineStatesJson["pointStates"].size());

	for (size_t i = 0; i < pointStates.size(); i++) {
		pointStates[i].SetFromJson(splineStatesJson["pointStates"][i]);
	}
}
}//namespace game
}//namespace poke