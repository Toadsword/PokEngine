#include <GraphicsEngine/Gizmos/gizmo_instances.h>
#include <GraphicsEngine/Models/mesh.h>
#include <GraphicsEngine/Pipelines/pipeline_graphic.h>
#include <GraphicsEngine/graphic_engine.h>
#include <math/hash.h>

namespace poke {
namespace graphics {
VertexInput GizmoInstances::Instance::GetVertexInput(const uint32_t baseBinding)
{
    const std::vector<VkVertexInputBindingDescription>
        bindingDescription = {
            VkVertexInputBindingDescription{
                baseBinding,
                sizeof(Instance),
                VK_VERTEX_INPUT_RATE_INSTANCE
            }
        };

    const std::vector<VkVertexInputAttributeDescription>
        attributeDescriptions = {
            VkVertexInputAttributeDescription{
                0,
                baseBinding,
                VK_FORMAT_R32G32B32A32_SFLOAT,
                offsetof(Instance, modelMatrix)
            },
            VkVertexInputAttributeDescription{
                1,
                baseBinding,
                VK_FORMAT_R32G32B32A32_SFLOAT,
                offsetof(Instance, modelMatrix) + 16
            },
            VkVertexInputAttributeDescription{
                2,
                baseBinding,
                VK_FORMAT_R32G32B32A32_SFLOAT,
                offsetof(Instance, modelMatrix) + 32
            },
            VkVertexInputAttributeDescription{
                3,
                baseBinding,
                VK_FORMAT_R32G32B32A32_SFLOAT,
                offsetof(Instance, modelMatrix) + 48
            },
            VkVertexInputAttributeDescription{
                4,
                baseBinding,
                VK_FORMAT_R32G32B32A32_SFLOAT,
                offsetof(Instance, color)
            },
        };
    return VertexInput(0, bindingDescription, attributeDescriptions);
}

std::unique_ptr<GizmoInstances> GizmoInstances::Create(
    const Mesh& mesh,
    float lineThickness)
{
    return std::make_unique<GizmoInstances>(mesh, lineThickness);
}

GizmoInstances::GizmoInstances(
    const Mesh& mesh,
    const float lineThickness)
    : kMesh_(mesh),
      lineThickness_(lineThickness),
      maxInstances_(0),
      instances_(0),
      instanceBuffer_(sizeof(Instance) * kMaxInstances) {}

GizmoInstances::~GizmoInstances()
{
    //std::cout << "~GizmoInstance\n";
}

void GizmoInstances::Update(std::vector<GizmoDrawCommand>& gizmos)
{
    maxInstances_ = kMaxInstances;
    instances_ = 0;

    if (gizmos.empty()) { return; }

    Instance* instancesPtr;
    instanceBuffer_.MapMemory(reinterpret_cast<char**>(&instancesPtr));

    for (auto& gizmo : gizmos) {
        if (instances_ >= maxInstances_) { break; }

        auto instance = &instancesPtr[instances_];
        instance->modelMatrix = gizmo.worldMatrix;
        instance->color = gizmo.c;
        instances_++;
    }

    instanceBuffer_.UnmapMemory();
}

bool GizmoInstances::CmdRender(
    const CommandBuffer& commandBuffer,
    const PipelineGraphics& pipeline,
    UniformHandle& uniformScene)
{
    if (instances_ == 0) { return false; }

	{
		const std::string name = "UniformScene";
		const auto hash = math::HashString(name);
		descriptorSet_.Push(hash, uniformScene);
	}

    if (!descriptorSet_.Update(pipeline)) {
        return false;
    }

    vkCmdSetLineWidth(commandBuffer, lineThickness_);

    descriptorSet_.BindDescriptor(commandBuffer, pipeline);

    VkBuffer vertexBuffers[] = {
        kMesh_.GetVertexBuffer().GetBuffer(),
        instanceBuffer_.GetBuffer()
    };
    VkDeviceSize offset[] = {0, 0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 2, vertexBuffers, offset);
    vkCmdBindIndexBuffer(
        commandBuffer,
        kMesh_.GetIndexBuffer().GetBuffer(),
        0,
        Mesh::GetIndexType());
    vkCmdDrawIndexed(
        commandBuffer,
        kMesh_.GetIndexCount(),
        instances_,
        0,
        0,
        0);

    return true;
}

void GizmoInstances::SetLineThickness(const float& lineThickness)
{
    lineThickness_ = lineThickness;
}
} //namespace graphics
} //namespace poke
