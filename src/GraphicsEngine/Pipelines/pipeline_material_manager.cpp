#include <GraphicsEngine/Pipelines/pipeline_material_manager.h>

#include <CoreEngine/cassert.h>

namespace poke {
namespace graphics {
PipelineMaterial& PipelineMaterialManager::GetMaterial(
    const Pipeline::Stage pipelineStage,
    const PipelineGraphicsCreateInfo& pipelineCreate) const
{
    auto i = 0;
    for (const auto& infoMaterial : registeredInfos_) {
        if (infoMaterial.first == pipelineStage && infoMaterial.second ==
            pipelineCreate) { return *registeredMaterials_[i]; }
        i++;
    }
    cassert(false, "try accessing non existing material");
}

PipelineMaterial& PipelineMaterialManager::AddMaterial(
    Pipeline::Stage pipelineStage,
    const PipelineGraphicsCreateInfo& pipelineCreate)
{
	for (size_t i = 0; i < registeredInfos_.size(); i++) {
        if(registeredInfos_[i].second == pipelineCreate) {
			return *registeredMaterials_[i];
        }
    }

    registeredMaterials_.push_back(
        std::make_unique<PipelineMaterial>(pipelineStage, pipelineCreate));

    registeredInfos_.emplace_back(pipelineStage, pipelineCreate);
    return *registeredMaterials_[registeredMaterials_.size() - 1];
}
} // namespace graphics
} // namespace poke
