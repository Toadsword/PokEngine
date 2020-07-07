#include <Ecs/Archetypes/archetype.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace ecs {
Archetype::Archetype() {
	transform.SetLocalPosition(math::Vec3{ 1000000 });
}

void Archetype::SetFromJson(const json& archetypeJson)
{
	//Editor Component
	if (CheckJsonExists(archetypeJson, "objectName")) {
		const std::string name = archetypeJson["objectName"];
		editorComponent.name = name;
		AddComponent(ComponentType::EDITOR_COMPONENT);
	}
	
	//Transform
	if (CheckJsonExists(archetypeJson, "transform")) {
		transform.SetFromJson(archetypeJson["transform"]);
		AddComponent(ComponentType::TRANSFORM);
	}

	//Model
	if (CheckJsonExists(archetypeJson, "model")) {
		model.SetFromJson(archetypeJson["model"]);
		AddComponent(ComponentType::MODEL);
	}

	//Spline
	if (CheckJsonExists(archetypeJson, "spline")) {
		spline.SetFromJson(archetypeJson["spline"]);
		AddComponent(ComponentType::SPLINE_FOLLOWER);
	}

	//Rigidbody
	if (CheckJsonExists(archetypeJson, "rigidbody")) {
		rigidbody.SetFromJson(archetypeJson["rigidbody"]);
		AddComponent(ComponentType::RIGIDBODY);
	}

	//Collider
	if (CheckJsonExists(archetypeJson, "collider")) {
		collider.SetFromJson(archetypeJson["collider"]);
	    AddComponent(ComponentType::COLLIDER);
	}

	//Light
	if (CheckJsonExists(archetypeJson, "light")) {
		light.SetFromJson(archetypeJson["light"]);
		AddComponent(ComponentType::LIGHT);
	}

	//particleSystem
	if (CheckJsonExists(archetypeJson, "particleSystem")) {
		particleSystem.SetFromJson(archetypeJson["particleSystem"]);
		AddComponent(ComponentType::PARTICLE_SYSTEM);
	}

	//Trail renderer
	if (CheckJsonExists(archetypeJson, "trailRenderer")) {
		trailRenderer.SetFromJson(archetypeJson["trailRenderer"]);
		AddComponent(ComponentType::TRAIL_RENDERER);
	}

	//AudioSource
	if (CheckJsonExists(archetypeJson, "audioSource")) {
		audioSource.SetFromJson(archetypeJson["audioSource"]);
		AddComponent(ComponentType::AUDIO_SOURCE);
	}

}

json Archetype::ToJson() const 
{
	json archetypeJson;

	//Transform
	if (HasComponent(ComponentType::TRANSFORM)) {
		archetypeJson["transform"] = transform.ToJson();
	}

	//Model
	if (HasComponent(ComponentType::MODEL)) {
		archetypeJson["model"] = model.ToJson();
	}

	//Spline
	if (HasComponent(ComponentType::SPLINE_FOLLOWER)) {
		archetypeJson["spline"] = spline.ToJson();
	}

	//Rigidbody
	if (HasComponent(ComponentType::RIGIDBODY)) {
		archetypeJson["rigidbody"] = rigidbody.ToJson();
	}

	//Collider
	if (HasComponent(ComponentType::COLLIDER)) {
		archetypeJson["collider"] = collider.ToJson();
	}
	//Light
	if (HasComponent(ComponentType::LIGHT)) {
		archetypeJson["light"] = light.ToJson();
	}
	//ParticleSystem
	if (HasComponent(ComponentType::PARTICLE_SYSTEM)) {
		archetypeJson["particleSystem"] = particleSystem.ToJson();
	}
	//AudioSource
	if (HasComponent(ComponentType::AUDIO_SOURCE)) {
		archetypeJson["audioSource"] = audioSource.ToJson();
	}

	//Trail renderer
	if (HasComponent(ComponentType::TRAIL_RENDERER)) {
		archetypeJson["trailRenderer"] = trailRenderer.ToJson();
	}

	return archetypeJson;
}
} //namespace ecs
} //namespace poke
