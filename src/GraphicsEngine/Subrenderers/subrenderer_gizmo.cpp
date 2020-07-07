#include <GraphicsEngine/Subrenderers/subrenderer_gizmo.h>
#include <GraphicsEngine/vertex_mesh.h>
#include <GraphicsEngine/graphic_engine.h>
#include <GraphicsEngine/Gizmos/gizmo_command_buffer.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {


SubrendererGizmo::SubrendererGizmo(const Pipeline::Stage stage)
    : RenderPipeline(stage),
      pipeline_(
          stage,
          "gizmo",
          {
              VertexMesh::GetVertexInput(0),
              GizmoInstances::Instance::GetVertexInput(1)
          },
          PipelineGraphics::Mode::POLYGON,
          PipelineGraphics::Depth::READ_WRITE,
          VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
          VK_POLYGON_MODE_LINE,
          VK_CULL_MODE_NONE,
          VK_FRONT_FACE_CLOCKWISE,
          false),
      gizmoManager_(GraphicsEngineLocator::Get().GetGizmoCommandBuffer()) {}

SubrendererGizmo::~SubrendererGizmo()
{
	std::cout << "~SubrendererGizmo();\n";
}

void SubrendererGizmo::OnRender(const CommandBuffer& commandBuffer)
{
    const auto camera = GraphicsEngineLocator::Get().GetCameraData();

    uniformScene_.Push(kProjectionHash, camera.projectionMatrix);
	uniformScene_.Push(kViewHash, camera.viewMatrix);

    pipeline_.BindPipeline(commandBuffer);

    for (auto& gizmoType : gizmoManager_.GetGizmoInstances()) {
        gizmoType.CmdRender(commandBuffer, pipeline_, uniformScene_);
    }
}
} //namespace graphics
} //namespace poke
