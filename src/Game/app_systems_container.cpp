#include <Game/app_systems_container.h>
#include <CoreEngine/engine.h>
#include <Utility/log.h>

namespace poke {
AppSystemsContainer::AppSystemsContainer(Engine& engine, game::Game& game) : engine_(engine), game_(game) {
	engine.AddObserver(
		observer::MainLoopSubject::APP_INIT,
		[this]() {this->OnGameInit(); }
	);
}

void AppSystemsContainer::OnGameInit() {
	playerController.emplace(engine_, game_);
	destructibleElementManager.emplace(engine_, game_);
	enemiesSystem.emplace(engine_, game_);
	weaponController.emplace(engine_, game_);
	projectileController.emplace(engine_, game_);
	mainMenu.emplace(engine_, game_);
	missilesSystem.emplace(engine_, game_);
	aimingAid.emplace(engine_, game_);
	specialAttackSystem.emplace(engine_, game_);
	jiggleSystem.emplace(engine_, game_);
	splineStateSystem.emplace(engine_, game_);
	gameCameraSystem.emplace(engine_, game_);
	demoSystem.emplace(engine_, game_);
	verticalSliceSystem.emplace(engine_, game_);
}
}//namespace poke