#include <Game/Components/game_camera.h>

namespace poke::game {

bool GameCamera::operator==(const GameCamera & other) const
{
	return this->playerOffset == other.playerOffset;
}
bool GameCamera::operator!=(const GameCamera & other) const
{
	return !(*this == other);
}

const json GameCamera::ToJson() const {
	json gameCameraJson;

    gameCameraJson["playerOffset"] = playerOffset.ToJson();

    return gameCameraJson;
}

void GameCamera::SetFromJson(const json& gameCameraJson) {
	playerOffset.SetFromJson(gameCameraJson["playerOffset"]);
}
}//namespace poke::game