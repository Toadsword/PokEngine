#include <Inputs/joystick.h>

namespace poke {
JoystickController::JoystickController() {
    if (SDL_Init(SDL_INIT_JOYSTICK) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Couldn't initialize SDL: %s\n",
                     SDL_GetError());
    }
}
} //namespace poke
