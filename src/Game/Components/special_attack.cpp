#include <Game/Components/special_attack.h>

namespace poke::game
{
bool SpecialAttack::operator==(const SpecialAttack & other) const
{
	return this->expansionSpeed == other.expansionSpeed &&
		this->maxRadius == other.maxRadius;
}
bool SpecialAttack::operator!=(const SpecialAttack & other) const
{
	return !(*this == other);
}

json SpecialAttack::ToJson() const {
	json j = json{
		{"expansionSpeed", expansionSpeed},
		{"maxRadius", maxRadius}
	};
	return j;
}

void SpecialAttack::SetFromJson(const json& specialAttackJson) {
	expansionSpeed = specialAttackJson["expansionSpeed"];
	maxRadius = specialAttackJson["maxRadius"];
}
}//namespace poke::game