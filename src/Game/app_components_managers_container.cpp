#include <Game/app_components_managers_container.h>

// app component part
#include <Game/ComponentManagers/player_manager.h>
#include <Game/ComponentManagers/destructible_element_manager.h>
#include <Game/ComponentManagers/weapon_manager.h>
#include <Game/ComponentManagers/projectile_manager.h>
#include <Game/ComponentManagers/enemies_manager.h>
#include <Game/ComponentManagers/missiles_manager.h>
#include <Game/ComponentManagers/special_attack_manager.h>
#include <Game/ComponentManagers/spline_state_manager.h>
#include <Game/ComponentManagers/jiggle_manager.h>
#include <Game/ComponentManagers/game_camera_manager.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace ecs {
void AppComponentsManagersContainer::Init() {
	auto& ecsManager = EcsManagerLocator::Get();
	ecsManager.RegisterManager(game::EnemiesManager());
	ecsManager.RegisterManager(game::PlayerManager());
	ecsManager.RegisterManager(game::DestructibleElementManager());
	ecsManager.RegisterManager(game::WeaponManager());
    ecsManager.RegisterManager(game::ProjectileManager());
	ecsManager.RegisterManager(game::MissilesManager());
	ecsManager.RegisterManager(game::SplineStateManager());
	ecsManager.RegisterManager(game::SpecialAttackManager());
	ecsManager.RegisterManager(game::JiggleManager());
	ecsManager.RegisterManager(game::GameCameraManager());
}
}//namespace ecs
}//namespace poke