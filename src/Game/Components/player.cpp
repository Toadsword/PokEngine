#include <Game/Components/player.h>

namespace poke {
namespace game {
bool Player::operator==(const Player & other) const {
	return this->dashCooldown == other.dashCooldown &&
		this->life == other.life &&
		this->mode == other.mode &&
		this->score == other.score &&
		this->destructionPoint == other.destructionPoint &&
		this->missileAmount == other.missileAmount &&
		this->specialCapacityAvailableAt == other.specialCapacityAvailableAt &&
		this->maxLockingTargets == other.maxLockingTargets &&
		this->moveSpeed == other.moveSpeed &&
		this->maxMoveSpeed == other.maxMoveSpeed &&
		this->moveDirection == other.moveDirection &&
		this->dashSpeed == other.dashSpeed &&
		this->dashDuration == other.dashDuration &&
		this->dashCooldown == other.dashCooldown &&
        this->targetSightModelOne == other.targetSightModelOne &&
        this->targetSightModelTwo == other.targetSightModelTwo &&
        this->targetSightModelThree == other.targetSightModelThree &&
        this->targetSightPositionOne == other.targetSightPositionOne &&
        this->targetSightPositionTwo == other.targetSightPositionTwo &&
        this->targetSightPositionThree == other.targetSightPositionThree &&
		this->shieldLife == other.shieldLife &&
		this->maxShieldLife == other.maxShieldLife &&
		this->shieldRegenSpeed == other.shieldRegenSpeed &&
		this->timeBeforeShieldRegen == other.timeBeforeShieldRegen &&
		this->specialCapacityMaxRadius == other.specialCapacityMaxRadius &&
		this->currentSpecialCapacityRadius == other.currentSpecialCapacityRadius &&
		this->specialCapacityIncrementedBy == other.specialCapacityIncrementedBy;
}
	
bool Player::operator!=(const Player & other) const {
	return !(*this == other);
}

json Player::ToJson() const
{
    json playerJson = json::object();
    playerJson["life"] = life;
    playerJson["mode"] = mode;
    playerJson["score"] = score;
    playerJson["destructionPoint"] = destructionPoint;
    playerJson["specialCapacityAvailableAt"] = specialCapacityAvailableAt;
    playerJson["missileAmount"] = missileAmount;
    playerJson["maxLockingTargets"] = maxLockingTargets;
    playerJson["moveSpeed"] = moveSpeed;
    playerJson["maxMoveSpeed"] = maxMoveSpeed;
    playerJson["moveDirection"] = moveDirection.ToJson();
    playerJson["dashSpeed"] = dashSpeed;
    playerJson["dashDuration"] = dashDuration;
    playerJson["dashCooldown"] = dashCooldown;
	playerJson["timeBeforeShieldRegen"] = timeBeforeShieldRegen;
	playerJson["shieldRegenSpeed"] = shieldRegenSpeed;
	playerJson["shieldLife"] = shieldLife;
	playerJson["specialCapacityMaxRadius"] = specialCapacityMaxRadius;
	playerJson["specialCapacityIncrementedBy"] = specialCapacityIncrementedBy;
	playerJson["targetSightPositionOne"] = targetSightPositionOne;
	playerJson["targetSightPositionTwo"] = targetSightPositionTwo;
	playerJson["targetSightPositionThree"] = targetSightPositionThree;
	playerJson["targetSightModelOne"] = targetSightModelOne;
	playerJson["targetSightModelTwo"] = targetSightModelTwo;
	playerJson["targetSightModelThree"] = targetSightModelThree;
    return playerJson;
}

void Player::SetFromJson(const json& playerJson)
{
    life = playerJson["life"];
    maxLife = playerJson["life"];
    mode = playerJson["mode"];
    score = playerJson["score"];
    destructionPoint = playerJson["destructionPoint"];
	specialCapacityAvailableAt = playerJson["specialCapacityAvailableAt"];
    missileAmount = playerJson["missileAmount"];
	maxLockingTargets = playerJson["maxLockingTargets"];
    moveSpeed = playerJson["moveSpeed"];
    maxMoveSpeed = playerJson["maxMoveSpeed"];
    moveDirection.SetFromJson(playerJson["moveDirection"]);
    dashSpeed = playerJson["dashSpeed"];
    dashDuration = playerJson["dashDuration"];
    dashCooldown = playerJson["dashCooldown"];
	timeBeforeShieldRegen = playerJson["timeBeforeShieldRegen"];
	shieldRegenSpeed = playerJson["shieldRegenSpeed"];
	shieldLife = playerJson["shieldLife"];
	maxShieldLife = playerJson["shieldLife"];
	specialCapacityMaxRadius = playerJson["specialCapacityMaxRadius"];
	specialCapacityIncrementedBy = playerJson["specialCapacityIncrementedBy"];
	targetSightPositionOne = playerJson["targetSightPositionOne"];
	targetSightPositionTwo = playerJson["targetSightPositionTwo"];
	targetSightPositionThree = playerJson["targetSightPositionThree"];
	targetSightModelOne = playerJson["targetSightModelOne"];
	targetSightModelTwo = playerJson["targetSightModelTwo"];
	targetSightModelThree = playerJson["targetSightModelThree"];
}
}//namespace game
}//namespace poke