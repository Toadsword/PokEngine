#include <CoreEngine/module_container.h>
#include <CoreEngine/engine.h>

namespace poke {
ModuleContainer::ModuleContainer(Engine& engine)
    : graphicsEngine(engine),
      inputManager(engine),
      sceneManager(engine),
      //ecsManager(engine, engine.GetEngineSettings().GetDefaultPoolSize()),
      physicsEngine(),
      resourcesManagerContainer(engine),
      chunksManager(engine),
      tagManager(engine),
      archetypesManager(){}
} //namespace poke
