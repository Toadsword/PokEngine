#include <Game/Components/destructible_element.h>

namespace poke {
namespace game {

bool DestructibleElement::operator==(const DestructibleElement & other) const
{
	return this->life == other.life &&
		this->scorePointGiven == other.scorePointGiven &&
		this->destructionPointGiven == other.destructionPointGiven &&
		this->type == other.type &&
		this->isIndestructible == other.isIndestructible;
}
bool DestructibleElement::operator!=(const DestructibleElement & other) const
{
	return !(*this == other);
}

const json DestructibleElement::ToJson() const {
    json destructibleElementJson = json::object();
    destructibleElementJson["life"] = life;
    destructibleElementJson["scorePointGiven"] = scorePointGiven;
    destructibleElementJson["destructionPointGiven"] = destructionPointGiven;
	destructibleElementJson["type"] = static_cast<int>(type);
	destructibleElementJson["isIndestructible"] = isIndestructible;
    return destructibleElementJson;
}

void DestructibleElement::SetFromJson(const json& destructibleElementJson) {
    life = destructibleElementJson["life"];
    scorePointGiven = destructibleElementJson["scorePointGiven"];
    destructionPointGiven = destructibleElementJson["destructionPointGiven"];
	type = destructibleElementJson["type"];
	isIndestructible = destructibleElementJson["isIndestructible"];
}
}//namespace game
}//namespace poke