#include <CoreEngine/engine_application.h>

namespace poke {
EngineApplication::EngineApplication(
    Engine& engine,
    const std::string& fileName)
    : Module(engine),
      appFileName_(fileName),
      camera_(engine) { }
} //namespace poke
