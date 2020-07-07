#include <Game/Components/jiggle.h>

namespace poke {
namespace game {

bool Jiggle::operator==(const Jiggle& jiggle) const {
	return rotationSpeed == jiggle.rotationSpeed &&
		maxTime == jiggle.maxTime &&
		speed == jiggle.speed;
}

bool Jiggle::operator!=(const Jiggle& jiggle) const {
	return !(*this == jiggle);
}

const json Jiggle::ToJson() const {
	json pointStateJson = json::object();
	pointStateJson["rotationSpeed"] = rotationSpeed;
	pointStateJson["speed"] = speed;
	pointStateJson["maxTime"] = maxTime;
	return pointStateJson;
}
void Jiggle::SetFromJson(const json& jiggleJson) { 
	rotationSpeed = jiggleJson["rotationSpeed"];
	speed = jiggleJson["speed"];
	maxTime = jiggleJson["maxTime"];
}
}//namespace game
}//namespace poke