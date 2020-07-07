#include <Game/Ecs/game_archetype.h>

namespace poke {
namespace game {

void GameArchetype::SetFromJson(const json& archetypeJson)
{
    //Engine Part
	ecs::Archetype::SetFromJson(archetypeJson);

    //Player
	if (CheckJsonExists(archetypeJson, "player")) {
		AddComponent(ecs::ComponentType::PLAYER);
		player.SetFromJson(archetypeJson["player"]);
	}

    //Destructible Element
	if (CheckJsonExists(archetypeJson, "destructibleElement")) {
		AddComponent(ecs::ComponentType::DESTRUCTIBLE_ELEMENT);
		destructibleElement.SetFromJson(archetypeJson["destructibleElement"]);
	}

    //enemy
	if (CheckJsonExists(archetypeJson, "enemy")) {
		AddComponent(ecs::ComponentType::ENEMY);
		enemy.SetFromJson(archetypeJson["enemy"]);
	}

    //Missile
	if (CheckJsonExists(archetypeJson, "missile")) {
		missile.SetFromJson(archetypeJson["missile"]);
		AddComponent(ecs::ComponentType::MISSILE);
	}

    //Projectile
	if (CheckJsonExists(archetypeJson, "projectile")) {
		projectile.SetFromJson(archetypeJson["projectile"]);
		AddComponent(ecs::ComponentType::PROJECTILE);
	}

	//Weapon
	if (CheckJsonExists(archetypeJson, "weapon")) {
		weapon.SetFromJson(archetypeJson["weapon"]);
		AddComponent(ecs::ComponentType::WEAPON);
	}

	//Spline States
	if (CheckJsonExists(archetypeJson, "splineStates")) {
		splineStates.SetFromJson(archetypeJson["splineStates"]);
		AddComponent(ecs::ComponentType::SPLINE_STATES);
	}

	//specialAttackIndex
	if (CheckJsonExists(archetypeJson, "specialAttack")) {
		specialAttack.SetFromJson(archetypeJson["specialAttack"]);
		AddComponent(ecs::ComponentType::SPECIAL_ATTACK);
	}

	//Jiggle component
	if (CheckJsonExists(archetypeJson, "jiggle")) {
		jiggle.SetFromJson(archetypeJson["jiggle"]);
		AddComponent(ecs::ComponentType::JIGGLE);
	}

	//Game Camera component
	if (CheckJsonExists(archetypeJson, "gameCamera")) {
		gameCamera.SetFromJson(archetypeJson["gameCamera"]);
		AddComponent(ecs::ComponentType::GAME_CAMERA);
	}
}

json GameArchetype::ToJson() const
{
    json archetypeJson = Archetype::ToJson();

	if (HasComponent(ecs::ComponentType::PLAYER)) {
		archetypeJson["player"] = player.ToJson();
	}

	if (HasComponent(ecs::ComponentType::DESTRUCTIBLE_ELEMENT)) {
		archetypeJson["destructibleElement"] =
			destructibleElement.ToJson();
	}

	if (HasComponent(ecs::ComponentType::ENEMY)) {
		archetypeJson["enemy"] = enemy.ToJson();
	}

	if (HasComponent(ecs::ComponentType::MISSILE)) {
		archetypeJson["missile"] = missile.ToJson();
	}

	if (HasComponent(ecs::ComponentType::PROJECTILE)) {
		archetypeJson["projectile"] = projectile.ToJson();
	}

	if (HasComponent(ecs::ComponentType::WEAPON)) {
		archetypeJson["weapon"] = weapon.ToJson();
	}

	//Spline States
    if(HasComponent(ecs::ComponentType::SPLINE_STATES)) {
		archetypeJson["splineStates"] = splineStates.ToJson();
    }

	//specialAttack
	if (HasComponent(ecs::ComponentType::SPECIAL_ATTACK)) {
		archetypeJson["specialAttack"] = specialAttack.ToJson();
	}

	//Jiggle
	if (HasComponent(ecs::ComponentType::JIGGLE)) {
		archetypeJson["jiggle"] = jiggle.ToJson();
	}
	//Game camera
	if (HasComponent(ecs::ComponentType::GAME_CAMERA)) {
		archetypeJson["gameCamera"] = gameCamera.ToJson();
	}

	return archetypeJson;
}
} //namespace game
} //namespace poke