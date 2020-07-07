#include <Editor/Tools/graphics_controller_tool.h>

#include <imgui.h>
#include "CoreEngine/ServiceLocator/service_locator_definition.h"
#include "GraphicsEngine/Pipelines/pipeline_blur.h"

namespace poke {
namespace editor {

GraphicControllerTool::GraphicControllerTool(Editor& editor, bool defaultActive)
    : Tool(defaultActive)
{
    isActive_ = defaultActive;
    name_ = "Graphic Controller";
    toolFlag_ = TOOL_FLAG_GRAPHIC_CONTROLLER;
}

void GraphicControllerTool::OnDrawImgui()
{
	ImGui::Begin("Graphic controller", &isActive_);

	auto& pipelineBlur = GraphicsEngineLocator::Get().GetRenderer().GetRendererContainer().Get<graphics::PipelineBlur>();

	float blur = pipelineBlur.GetBlur();
	ImGui::DragFloat("Blur effect", &blur, 0.001, 0, 10);
	pipelineBlur.SetBlur(blur);

	ImGui::End();
}

void GraphicControllerTool::OnSetInactive() {}

void GraphicControllerTool::OnSetActive() {}
} //namespace editor
} //namespace poke