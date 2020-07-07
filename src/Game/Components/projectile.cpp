#include <Game/Components/projectile.h>

namespace poke {
namespace game {

bool Projectile::operator==(const Projectile & other) const
{
	return this->damage == other.damage &&
		this->moveSpeed == other.moveSpeed &&
		this->durationLifeTime == other.durationLifeTime;
}
bool Projectile::operator!=(const Projectile & other) const
{
	return !(*this == other);
}

const json Projectile::ToJson() const {
    json projectileJson = json::object();
    projectileJson["damage"] = damage;
    projectileJson["moveSpeed"] = moveSpeed;
    projectileJson["durationLifeTime"] = durationLifeTime;
	projectileJson["shootAtTime"] = shootAtTime;
    return projectileJson;
}

void Projectile::SetFromJson(const json& projectileJson) {
    damage = projectileJson["damage"];
    moveSpeed = projectileJson["moveSpeed"];
    durationLifeTime = projectileJson["durationLifeTime"];
	shootAtTime = projectileJson["shootAtTime"];
}
}//namespace game
}//namespace poke