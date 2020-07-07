#include <GraphicsEngine/Subrenderers/subrenderer_transparent.h>
#include <GraphicsEngine/graphic_engine.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {
SubrendererTransparent::SubrendererTransparent(const Pipeline::Stage stage)
    : RenderPipeline(stage),
      uniformScene_(true),
      modelCmdBuffer_(
          GraphicsEngineLocator::Get().GetModelCommandBuffer()) {}

SubrendererTransparent::~SubrendererTransparent()
{
	//std::cout << "~SubrendererTransparent();\n";
}

void SubrendererTransparent::OnRender(const CommandBuffer& commandBuffer)
{
    const auto camera = GraphicsEngineLocator::Get().GetCameraData();

    uniformScene_.Push(kProjectionHash, camera.projectionMatrix);

	uniformScene_.Push(kViewHash, camera.viewMatrix);

	//Single Draw
	for (auto& modelDrawCommand : modelCmdBuffer_.GetForwardModels()) {
		if (modelDrawCommand.materialID != 0 && modelDrawCommand.materialID != 1) {
			if (MaterialsManagerLocator::Get().GetMaterial(modelDrawCommand.materialID).GetRenderMode() == Material::RenderMode::TRANSPARENT) {
				CmdRender(commandBuffer, modelDrawCommand);
			}
		}
	}

    for (auto& modelInstance : modelCmdBuffer_.GetModelInstances()) {
		if (modelInstance->GetMaterial().GetRenderMode() == Material::RenderMode::TRANSPARENT) {
			modelInstance->CmdRender(commandBuffer, uniformScene_);
		}
    }
}

bool SubrendererTransparent::CmdRender(
	const CommandBuffer& commandBuffer,
	ForwardDrawCmd& modelDrawCommand)
{
	modelDrawCommand.uniformObject.Push(kTransformHash, modelDrawCommand.worldMatrix);

	auto& mat = MaterialsManagerLocator::Get().GetMaterial(modelDrawCommand.materialID);
	modelDrawCommand.uniformObject.PushUniformData(mat.ExportUniformData());

	// Check if we are in the correct pipeline stage.
	const auto& materialPipeline = mat.GetPipelineMaterial();
	if (materialPipeline.GetStage() != GetStage())
		return false;

	// Binds the material pipeline.
	if (!mat.BindPipeline(commandBuffer))
		return false;

	const auto& pipeline = materialPipeline.GetPipeline();

	// Updates descriptors.
	modelDrawCommand.descriptorSet.Push(kUboSceneHash, uniformScene_);
	modelDrawCommand.descriptorSet.Push(kUboObjectHash, modelDrawCommand.uniformObject);

	modelDrawCommand.descriptorSet.PushDescriptorData(mat.ExportDescriptorData());

	if (!modelDrawCommand.descriptorSet.Update(pipeline))
		return false;

	// Draws the object.
	modelDrawCommand.descriptorSet.BindDescriptor(commandBuffer, pipeline);

	return MeshManagerLocator::Get().GetMesh(modelDrawCommand.meshID).CmdRender(commandBuffer);
}
} //namespace graphics
} //namespace poke
