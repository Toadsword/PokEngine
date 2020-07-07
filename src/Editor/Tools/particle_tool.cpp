#include <Editor/Tools/particle_tool.h>

#include <CoreEngine/engine.h>
//TODO(@Gui) Remove all non-used include.
#include <Utility/json_utility.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

#include <Editor/editor.h>

#include <misc/cpp/imgui_stdlib.h>


namespace poke::editor {

poke::editor::ParticleTool::ParticleTool(Editor & editor, const bool defaultActive)
	: Tool(defaultActive)
	, ecsManager_(editor.GetEditorEcsManager())
	, particleManager_(ecsManager_.GetComponentsManager<ecs::ParticleSystemsManager>()) {

	isActive_ = defaultActive;
	name_ = "Particle tool";
	toolFlag_ = TOOL_FLAG_PARTICLE;
}

void ParticleTool::OnSetActive() {
	GetAllParticleSystems();
}

void ParticleTool::OnSetInactive() {

}

void ParticleTool::OnDrawImgui() {
	DisplayParticlesSystems();	
	DisplayParticleComponent(selectedEntityIndex_);
}

void ParticleTool::DisplayParticlesSystems() {

	entities_ = ecsManager_.GetEntitiesWithComponents(ecs::ComponentType::PARTICLE_SYSTEM);

	ImGui::Begin(kParticlesName_, &isActive_);

	if (ImGui::Button("Create new particle system")) {
		ImGui::OpenPopup("new particle system");
	}

	if (ImGui::BeginPopup("new particle system")) {

		if (ImGui::MenuItem("Empty")) {

			ecsManager_.DestroyEntities(entities_);
			newParticleName_ = "new particle";
			entityName_ = newParticleName_;
			const ecs::EntityIndex entityIndex = ecsManager_.AddEntity();
			CreateNewParticleSystem(entityIndex);
			selectedEntityIndex_ = entityIndex;
		}
		else for (const std::string& particleName : particlesNames_) 
		{
			const size_t lastIndex = particleName.find_last_of('.');
			const std::string newName = particleName.substr(0, lastIndex);

			if (ImGui::MenuItem(newName.c_str())) {
				ecsManager_.DestroyEntities(entities_);
				newParticleName_ = newName;
				entityName_ = newParticleName_;
				LoadParticle(newName);
			}
		}
		ImGui::EndPopup();
	}
		
	ImGui::Text(newParticleName_.c_str());

	for (ecs::EntityIndex entity : entities_)
	{
		if (!entities_.empty()) { 
			//On right-click
			if (ImGui::BeginPopupContextItem(newParticleName_.c_str())) {

				if (ImGui::Button("Delete Entity")) {
					ecsManager_.DestroyEntity(entity);
					newParticleName_ = "";
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Save as new particle")) {
					ImGui::OpenPopup("Save as new particle");
				}
				if (ImGui::BeginPopupModal(
					"Save as new particle",
					&isActive_,
					ImGuiWindowFlags_AlwaysAutoResize)) {
					ImGui::InputTextWithHint(
					"particle Name"
					, "Enter particle name"
					, &entityName_);

				if (ImGui::Button("Cancel"))
					ImGui::CloseCurrentPopup();
					ImGui::SameLine();

				if (ImGui::Button("Save")) {
					SaveParticle(entity, json(), entityName_);
					newParticleName_ = entityName_;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
				}
				ImGui::EndPopup();
			}
		}
	}
	ImGui::End();
}

void ParticleTool::CreateNewParticleSystem(const ecs::EntityIndex entityIndex) 
{ 
	ecsManager_.AddComponent(entityIndex, ecs::ComponentType::PARTICLE_SYSTEM);
	CheckIfEntityIsParticle(entityIndex);
	currentParticles_[entityIndex] = json();
}

void ParticleTool::LoadParticle(const std::string& particleName) 
{
	json newParticle = PokFileSystem::ReadJsonFile(
		particleName,
		FileType::PARTICLE,
		FolderType::SAVE_IN_ROM);

	newParticle["fileName"] = particleName;

	const ecs::EntityIndex entityIndex =
		ecsManager_.AddEntity();
	CheckIfEntityIsParticle(entityIndex);
	currentParticles_[entityIndex] = newParticle;
	LoadParticleFromJson(newParticle, entityIndex);
	selectedEntityIndex_ = entityIndex;
}

void ParticleTool::LoadParticleFromJson(
	const json& particleJson,
	const ecs::EntityIndex
	entityIndex)
{
	const std::string jsonParameter = "particle system"; 

	ecsManager_.AddComponent(entityIndex, ecs::ComponentType::PARTICLE_SYSTEM);
	auto particle = particleManager_.GetComponent(entityIndex);
	particle.SetFromJson(particleJson[jsonParameter]);
	particleManager_.SetComponent(entityIndex, particle);
}

bool ParticleTool::CheckIfEntityIsParticle(
	const ecs::EntityIndex entityIndex) 
{
	if (entityIndex >= currentParticles_.size()) {
		currentParticles_.resize(entityIndex + 1);
	}
	return !currentParticles_[entityIndex].is_null();
}

void ParticleTool::DisplayParticleComponent(const ecs::EntityIndex entityIndex)
{
	if (ecsManager_.HasComponent(entityIndex, ecs::ComponentType::PARTICLE_SYSTEM)) {
		//Modify particle system
		ImGui::Begin("particle set");
		auto particle = particleManager_.GetComponent(entityIndex);
		auto newParticle = ComponentUtility::DisplayAndModifyParticleSystem(particle);
		particleManager_.SetComponent(entityIndex, newParticle);
		ImGui::End();
	}
}

void ParticleTool::SaveParticle(const ecs::EntityIndex entityIndex,
	const json& particleJson,
	const std::string& particleName)
{

	json newParticleJson = WriteParticle(entityIndex, particleName);

	PokFileSystem::WriteFile(particleName,
		newParticleJson,
		FileType::PARTICLE,
		FolderType::SAVE_IN_ROM);
	if (particleJson.is_null()) {
		newParticleJson["filename"] = particleName;
		CheckIfEntityIsParticle(entityIndex);
		currentParticles_[entityIndex] = newParticleJson;
	}
	else {
		for (auto& currentParticle : currentParticles_) {
			// Change particle for same entity
			if (currentParticle == particleJson) { currentParticle = newParticleJson; }
		}
	}
	particlesNames_ =
		PokFileSystem::GetAllFilesInDirectories(
			PokFileSystem::GetPath(FileType::PARTICLE,
				FolderType::SAVE_IN_ROM),
			true,
			false);
}

json ParticleTool::WriteParticle(
	const ecs::EntityIndex entityIndex,
	const std::string& particleName)const {

	json newParticleJson = json();
	newParticleJson["fileName"] = particleName;

	const std::string jsonParameter = "particle system";
	const auto particle = particleManager_.GetComponent(entityIndex);
	newParticleJson[jsonParameter] = particle.ToJson();

	return newParticleJson;
}
void ParticleTool::GetAllParticleSystems() {
	const std::string path = PokFileSystem::GetPath(FileType::PARTICLE, FolderType::SAVE_IN_ROM);
	particlesNames_ = PokFileSystem::GetAllFilesInDirectories(path, true, false);
}
}//namespace poke::editor



