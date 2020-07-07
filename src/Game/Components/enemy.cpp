#include <Game/Components/enemy.h>

namespace poke {
namespace game {
	bool Enemy::operator==(const Enemy & other) const
	{
		bool isWeaponsSame = this->weapons.size() == other.weapons.size();
		if (isWeaponsSame) {
			for (size_t i = 0; i < other.weapons.size(); i++) {
				if (this->weapons[i] != other.weapons[i]) {
					isWeaponsSame = false;
					break;
				}
			}
		}

		return this->state == other.state &&
			this->approachSpeed == other.approachSpeed &&
			this->attackSpeed == other.attackSpeed &&
			this->fleeSpeed == other.fleeSpeed &&
			this->splineFireStartPoint == other.splineFireStartPoint &&
			this->splineFireEndPoint == other.splineFireEndPoint &&
			isWeaponsSame;
	}

bool Enemy::operator!=(const Enemy & other) const
{
	return !(*this == other);
}

const json Enemy::ToJson() const {
	json enemyJson = json::object();
	enemyJson["state"] = static_cast<int>(state);
	enemyJson["approachSpeed"] = approachSpeed;
	enemyJson["attackSpeed"] = attackSpeed;
	enemyJson["fleeSpeed "] = fleeSpeed;
	enemyJson["splineFireStartPoint"] = splineFireStartPoint;
	enemyJson["splineFireEndPoint"] = splineFireEndPoint;

	enemyJson["weaponsIndex"] = nlohmann::detail::value_t::array;
	for (size_t i = 0; i < weapons.size(); i++) {
		enemyJson["weaponsIndex"][i] = weapons[i];
	}
	return enemyJson;
}

void Enemy::SetFromJson(const json& enemyJson) {
	state = enemyJson["state"];
    approachSpeed = enemyJson["approachSpeed"];
	attackSpeed = enemyJson["attackSpeed"];
	fleeSpeed = enemyJson["fleeSpeed "];
	splineFireStartPoint = enemyJson["splineFireStartPoint"];
	splineFireEndPoint = enemyJson["splineFireEndPoint"];

	if (CheckJsonExists(enemyJson, "weaponsIndex")) {
		weapons.clear();
		json weaponsIndexData = enemyJson["weaponsIndex"];
		for (size_t i = 0; i < weaponsIndexData.size(); i++) {
			weapons.push_back(weaponsIndexData[i]);
		}
	}
}
}//namespace game
}//namespace poke