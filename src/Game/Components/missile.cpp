#include <Game/Components/missile.h>

namespace poke {
namespace game {

bool Missile::operator==(const Missile & other) const
{
	return this->direction == other.direction &&
		this->target == other.target &&
		this->speed == other.speed &&
		this->lifeTime == other.lifeTime &&
		this->currentLifeTime == other.currentLifeTime &&
		this->shootDelay == other.shootDelay &&
		this->hasBeenShot == other.hasBeenShot &&
		this->damage == other.damage;
}
bool Missile::operator!=(const Missile & other) const
{
	return !(*this == other);
}

const json Missile::ToJson() const {
	json missileJson;
	missileJson["direction"] = direction.ToJson();
	missileJson["target"] = target;
	missileJson["speed"] = speed;
	missileJson["lifeTime"] = lifeTime;
	missileJson["currentLifeTime"] = currentLifeTime;
	missileJson["shootDelay"] = shootDelay;
	missileJson["hasBeenShot"] = hasBeenShot;
	missileJson["damage"] = damage;
	return missileJson;
}
void Missile::SetFromJson(const json& missileJson) {
	direction.SetFromJson(missileJson["direction"]);
	target = missileJson["target"];
	speed = missileJson["speed"];
	lifeTime = missileJson["lifeTime"];
	currentLifeTime = missileJson["currentLifeTime"];
	shootDelay = missileJson["shootDelay"];
	hasBeenShot = missileJson["hasBeenShot"];
	damage = missileJson["damage"];
}
}//namespace game
}//namespace poke