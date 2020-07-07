#include <GraphicsEngine/Models/model_command_buffer.h>

#include <CoreEngine/engine.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {
ModelCommandBuffer::ModelCommandBuffer(Engine& engine)
{
    engine.AddObserver(
        observer::MainLoopSubject::ENGINE_INIT,
        [this]() { OnEngineInit(); });
}

ModelCommandBuffer::~ModelCommandBuffer()
{
    //std::cout << "~ModelCommandBuffer\n";
}

void ModelCommandBuffer::OnEngineInit()
{
    GraphicsEngineLocator::Get().GetEngine().GetModuleManager().sceneManager.
                                 AddOnUnloadObserver(
                                     [this]() { OnUnloadScene(); });
}

std::vector<std::unique_ptr<ModelInstance>>& ModelCommandBuffer::GetModelInstances()
{
    return modelInstances_;
}

ModelInstanceIndex ModelCommandBuffer::GetModelInstanceIndex(
    const Material& material,
    const Mesh& mesh)
{
    for (size_t i = 0; i < modelInstances_.size(); i++) {
        if (&modelInstances_[i]->GetMaterial() == &material) {
            if (&modelInstances_[i]->GetMesh() == &mesh) { return i; }
        }
    }

    modelInstances_.push_back(nullptr);
    modelInstances_.back() = std::make_unique<ModelInstance>(mesh, material);
    instancesMatrix_.push_back({});
    return modelInstances_.size() - 1;
}

ModelInstanceIndex ModelCommandBuffer::GetForwardIndex()
{

    //Find next free index
    for (size_t i = 0; i < forwardDrawingCmd_.size(); i++) {
        if (forwardDrawingCmd_[i].materialID == 0) {
			forwardDrawingCmd_[i].materialID = 1;
            return i;
        }
    }

	forwardDrawingCmd_.resize(forwardDrawingCmd_.size() * 2 + 2);

	for (size_t i = 0; i < forwardDrawingCmd_.size(); i++) {
		if (forwardDrawingCmd_[i].materialID == 0) {
			forwardDrawingCmd_[i].materialID = 1;
			return i;
		}
	}

    return 0;
}

void ModelCommandBuffer::FreeForwardIndex(const ModelForwardIndex index)
{
    forwardDrawingCmd_[index].materialID = 0;
}

void ModelCommandBuffer::Draw(
    const math::Matrix4 worldMatrix,
    const ModelInstanceIndex instanceIndex)
{
    if (instancesMatrix_[instanceIndex].size() == instancesMatrix_.capacity()) {
        instancesMatrix_[instanceIndex].resize(instancesMatrix_.size() * 2 + 1);
    }

    instancesMatrix_[instanceIndex].emplace_back(InstancingDrawCmd{worldMatrix});
}

void ModelCommandBuffer::Draw(
    const math::Matrix4 worldMatrix,
    const Model model,
    const ModelForwardIndex forwardIndex)
{
    forwardDrawingCmd_[forwardIndex].worldMatrix = worldMatrix;
    forwardDrawingCmd_[forwardIndex].materialID = model.materialID;
    forwardDrawingCmd_[forwardIndex].meshID = model.meshID;
}

void ModelCommandBuffer::Draw(const ForwardDrawCmd& drawCommand)
{
    forwardDrawingCmd_.push_back(drawCommand);
}

void ModelCommandBuffer::PrepareData()
{
    for (size_t i = 0; i < modelInstances_.size(); i++) {
        modelInstances_[i]->Update(instancesMatrix_[i]);

        instancesMatrix_[i].clear();
    }
}

void ModelCommandBuffer::OnUnloadScene()
{
    forwardDrawingCmd_.clear();
    forwardDrawingCmd_.shrink_to_fit();

    modelInstances_.clear();
}

void ModelCommandBuffer::Clear()
{
    forwardDrawingCmd_.clear();

    modelInstances_.clear();
}
} //namespace graphics
} //namespace poke
