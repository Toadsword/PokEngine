#include <Game/Components/weapon.h>

namespace poke {
namespace game {

bool Weapon::operator==(const Weapon & other) const
{

	bool isWeaponsSame = this->gunPositions.size() == other.gunPositions.size();
	if (isWeaponsSame) {
		for (size_t i = 0; i < other.gunPositions.size(); i++) {
			if (this->gunPositions[i] != other.gunPositions[i]) {
				isWeaponsSame = false;
				break;
			}
		}
	}

	return this->shootCoolDown == other.shootCoolDown &&
		//this->gunPositions == other.gunPositions &&
		this->weaponType == other.weaponType &&
		isWeaponsSame;
}
bool Weapon::operator!=(const Weapon & other) const
{
	return !(*this == other);
}

const json Weapon::ToJson() const {
	json weaponJson;

    weaponJson["shootCoolDown"] = shootCoolDown;

	weaponJson["gunPositions"] = nlohmann::detail::value_t::array;
	for (size_t i = 0; i < gunPositions.size(); i++) {
		weaponJson["gunPositions"][i] = gunPositions[i];
	}

	weaponJson["lightIntensity"] = lightIntensity;
	weaponJson["lightDuration"] = lightDuration;
    weaponJson["weaponType"] = weaponType;

    return weaponJson;
}

void Weapon::SetFromJson(const json& weaponJson) {
    shootCoolDown = weaponJson["shootCoolDown"];

	if (CheckJsonExists(weaponJson, "gunPositions")) {
		gunPositions.clear();
		json gunPositionsData = weaponJson["gunPositions"];
		for (size_t i = 0; i < gunPositionsData.size(); i++) {
			gunPositions.push_back(gunPositionsData[i]);
		}
	}

	lightIntensity = weaponJson["lightIntensity"];
	lightDuration = weaponJson["lightDuration"];
	weaponType = weaponJson["weaponType"];

}
}//namespace game
}//namespace poke