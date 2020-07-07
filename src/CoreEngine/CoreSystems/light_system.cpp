#include <CoreEngine/CoreSystems/light_system.h>

#include <CoreEngine/engine.h>
#include <Utility/profiler.h>

namespace poke {

LightSystem::LightSystem(Engine& engine)
    : System(engine),
      lightsManager_(ecsManager_.GetComponentsManager<ecs::LightsManager>()),
      pointLights_(100),
      spotLights_(100)
{
    engine_.AddObserver(
        observer::MainLoopSubject::DRAW,
        [this]() { OnDraw(); });
    engine_.AddObserver(
        observer::MainLoopSubject::CULLING,
        [this]() { OnCulling(); });
	engine_.AddObserver(
		observer::MainLoopSubject::UPDATE,
		[this]() { OnUpdate(); });

	engine_.AddObserver(
		observer::MainLoopSubject::END_FRAME,
		[this]() { OnEndOfFrame(); });

    ecsManager_.RegisterObserverAddComponent(
        [this](
        const ecs::EntityIndex entityIndex,
        const ecs::ComponentMask component) {
            OnEntityAddComponent(entityIndex, component);
        });
    ecsManager_.RegisterObserverRemoveComponent(
        [this](
        const ecs::EntityIndex entityIndex,
        const ecs::ComponentMask component) {
            OnEntityRemoveComponent(entityIndex, component);
        });
	ecsManager_.RegisterObserverUpdateComponent(
		[this](
			const ecs::EntityIndex entityIndex,
			const ecs::ComponentMask component) {
		OnComponentUpdate(entityIndex, component);
	});

    spotLightDrawCmds_.reserve(graphics::LightCommandBuffer::maxSpotLight);
    pointLightDrawCmds_.reserve(graphics::LightCommandBuffer::maxPointLight);
	SceneManagerLocator::Get().AddOnUnloadObserver([this]() {OnUnloadScene(); });

	ObserveEntitySetActive();
	ObserveEntitySetInactive();
}

void LightSystem::OnEntitySetActive(const ecs::EntityIndex entityIndex)
{
	if (ecsManager_.HasComponent(entityIndex, ecs::ComponentType::LIGHT)) {
		const auto& lightComponent = lightsManager_.GetComponent(entityIndex);

		switch (lightComponent.lightType) {
		case graphics::LightType::DIRECTIONAL:
			directionalLight_ = entityIndex;
			break;
		case graphics::LightType::SPOT:
			if (spotLights_.exist(entityIndex)) return;
			spotLights_.insert(entityIndex);
			break;
		case graphics::LightType::POINT:
			if (pointLights_.exist(entityIndex)) return;
			pointLights_.insert(entityIndex);
			break;
		default:;
		}
	}
}

void LightSystem::OnEntitySetInactive(const ecs::EntityIndex entityIndex)
{
	if (ecsManager_.HasComponent(entityIndex, ecs::ComponentType::LIGHT)) {
		//If its a spot light then erase it
		if (spotLights_.exist(entityIndex)) {
			spotLights_.erase(spotLights_.find(entityIndex));
		}
		else if (pointLights_.exist(entityIndex)) {
			pointLights_.erase(pointLights_.find(entityIndex));
		}
		else {
			//TODO(@Nico) Handle removing of directional light
		}
	}
}

void LightSystem::OnUpdate()
{
	pok_BeginProfiling(Light_System, 0);

	pok_EndProfiling(Light_System);
}

void LightSystem::OnUnloadScene()
{
	pointLightDrawCmds_.clear();
	spotLightDrawCmds_.clear();
}

void LightSystem::OnDraw()
{
	pok_BeginProfiling(Light_System, 0);
    auto& transformManager = ecsManager_.GetComponentsManager<ecs::TransformsManager>();

    //Spot lights
    for (auto entityIndex : spotLights_) {
        const auto light = lightsManager_.GetComponent(entityIndex);
        const auto worldPosition = transformManager.GetWorldPosition(
            entityIndex);
		if (light.spotLight.intensity > 0.0f) {
			spotLightDrawCmds_.emplace_back(
				graphics::SpotLightDrawCommand{
					light.spotLight.color * light.spotLight.intensity,
					worldPosition,
					light.spotLight.range,
					worldPosition + light.spotLight.direction,
					light.spotLight.angleInDeg
				});
		}
    }

    //Point lights
    for (auto entityIndex : pointLights_) {
        const auto light = lightsManager_.GetComponent(entityIndex);
        const auto worldPosition = transformManager.GetWorldPosition(
            entityIndex);

		if (light.spotLight.intensity > 0.0f) {
			pointLightDrawCmds_.emplace_back(
				graphics::PointLightDrawCommand{
					light.pointLight.color * light.pointLight.intensity,
					worldPosition,
					std::max(
						light.pointLight.areaSize.x,
						light.pointLight.areaSize.y)
				});
		}
    }

    //Directional light
    {
        const auto light = lightsManager_.GetComponent(directionalLight_);
		directionalLightDrawCmd_ = graphics::DirectionalLightDrawCommand{
				light.directionalLight.color  * light.directionalLight.intensity,
				light.directionalLight.direction
		};
    }
	pok_EndProfiling(Light_System);
}

void LightSystem::OnCulling()
{
	pok_BeginProfiling(Light_System, 0);
    //Sort visible
    //TODO(@Nico) Complete this part.

    //Sort closer
    //TODO(@Nico) Complete this part.
    //
    //Draw
    auto& lightCommandBuffer = GraphicsEngineLocator::Get().GetLightCommandBuffer();

    for (const auto& spotLightDrawCmd : spotLightRenderCmds_) {
        lightCommandBuffer.Draw(spotLightDrawCmd);
    }

    for (const auto& pointLightDrawCmd : pointLightRenderCmds_) {
        lightCommandBuffer.Draw(pointLightDrawCmd);
    }

    lightCommandBuffer.Draw(directionalLightRenderCmd_);

    spotLightRenderCmds_.clear();
    pointLightRenderCmds_.clear();
	pok_EndProfiling(Light_System);
}

void LightSystem::OnEndOfFrame()
{
	pok_BeginProfiling(Light_System, 0);
	spotLightRenderCmds_ = spotLightDrawCmds_;
	pointLightRenderCmds_ = pointLightDrawCmds_;
	directionalLightRenderCmd_ = directionalLightDrawCmd_;

	spotLightDrawCmds_.clear();
	pointLightDrawCmds_.clear();
	pok_EndProfiling(Light_System);
}

void LightSystem::OnEntityAddComponent(
    const ecs::EntityIndex entityIndex,
    const ecs::ComponentMask component)
{
    if (ecs::ComponentType::LIGHT == (component & ecs::ComponentType::LIGHT) &&
		ecsManager_.IsEntityActive(entityIndex)) {
        const auto& lightComponent = lightsManager_.GetComponent(entityIndex);

        switch (lightComponent.lightType) {
        case graphics::LightType::DIRECTIONAL:
            directionalLight_ = entityIndex;
            break;
        case graphics::LightType::SPOT:
			if (spotLights_.exist(entityIndex)) return;
            spotLights_.insert(entityIndex);
            break;
        case graphics::LightType::POINT:
			if (pointLights_.exist(entityIndex)) return;
            pointLights_.insert(entityIndex);
            break;
        default: ;
        }
    }
}

void LightSystem::OnEntityRemoveComponent(
    const ecs::EntityIndex entityIndex,
    const ecs::ComponentMask component)
{
    if (ecs::ComponentType::LIGHT == (component & ecs::ComponentType::LIGHT)) {
        //If its a spot light then erase it
        if(spotLights_.exist(entityIndex)) {
			spotLights_.erase(spotLights_.find(entityIndex));
        }else if(pointLights_.exist(entityIndex)) {
			pointLights_.erase(pointLights_.find(entityIndex));
        }else {
           //TODO(@Nico) Handle removing of directional light
        }
    }
}

void LightSystem::OnComponentUpdate(const ecs::EntityIndex entityIndex, const ecs::ComponentMask component)
{
	if (ecs::ComponentType::LIGHT == (component & ecs::ComponentType::LIGHT)) {
		const auto lightType = lightsManager_.GetComponent(entityIndex).lightType;

		//Check if a light type has changed
        if(pointLights_.exist(entityIndex)) {
            if(lightType == graphics::LightType::SPOT) {
				pointLights_.erase(pointLights_.find(entityIndex));
				spotLights_.insert(entityIndex);
            }
        } else if(spotLights_.exist(entityIndex)) {
			if (lightType == graphics::LightType::POINT) {
				spotLights_.erase(spotLights_.find(entityIndex));
				pointLights_.insert(entityIndex);
			}
        }
	}
}
} //namespace poke
