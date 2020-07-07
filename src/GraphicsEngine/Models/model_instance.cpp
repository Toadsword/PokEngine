#include <GraphicsEngine/Models/model_instance.h>

#include <GraphicsEngine/graphic_engine.h>

namespace poke {
namespace graphics {
VertexInput ModelInstance::Instance::GetVertexInput(const uint32_t baseBinding)
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
        };
    return VertexInput(0, bindingDescription, attributeDescriptions);
}

std::unique_ptr<ModelInstance> ModelInstance::Create(
    const Mesh& mesh,
    const Material& material
) { return std::make_unique<ModelInstance>(mesh, material); }

ModelInstance::ModelInstance(
    const Mesh& mesh,
    const Material& material)
    : kMesh_(mesh),
      kMaterial_(material),
      instanceBuffer_(sizeof(Instance) * kMaxInstances),
      uniformObject_(false) { }

void ModelInstance::Update(std::vector<InstancingDrawCmd>& models)
{
    maxInstances_ = kMaxInstances;
    instances_ = 0;

    if (models.empty()) { return; }

    Instance* instances;
    instanceBuffer_.MapMemory(reinterpret_cast<char**>(&instances));

    for (auto& model : models) {
        if (instances_ >= maxInstances_) { break; }

        auto instance = &instances[instances_];
        instance->modelMatrix = model.worldMatrix;
        instances_++;
    }

    instanceBuffer_.UnmapMemory();
}

bool ModelInstance::CmdRender(
    const CommandBuffer& commandBuffer,
    UniformHandle& uniformScene)
{
    if (instances_ == 0)
        return false;

    // Check if we are in the correct pipeline stage.
    const auto& materialPipeline = kMaterial_.GetPipelineMaterial();

    // Binds the material pipeline.
    if (!kMaterial_.BindPipeline(commandBuffer))
        return false;

    const auto& pipeline = materialPipeline.GetPipeline();

    uniformObject_.PushUniformData(kMaterial_.ExportUniformData());

    descriptorSet_.Push(kUniformObjectHash, uniformObject_);
    descriptorSet_.Push(kUniformSceneHash, uniformScene);

    descriptorSet_.PushDescriptorData(kMaterial_.ExportDescriptorData());

    if (!descriptorSet_.Update(pipeline))
        return false;

    //BIND DESCRIPTOR
    descriptorSet_.BindDescriptor(commandBuffer, pipeline);

    VkBuffer vertexBuffers[] = {
        kMesh_.GetVertexBuffer().GetBuffer(),
        instanceBuffer_.GetBuffer()
    };
    VkDeviceSize offset[] = {0, 0};

    //BIND VERTEX
    vkCmdBindVertexBuffers(commandBuffer, 0, 2, vertexBuffers, offset);

    //BIND INDEX
    vkCmdBindIndexBuffer(
        commandBuffer,
        kMesh_.GetIndexBuffer().GetBuffer(),
        0,
        Mesh::GetIndexType());

    //DRAW
    vkCmdDrawIndexed(
        commandBuffer,
        kMesh_.GetIndexCount(),
        instances_,
        0,
        0,
        0);

    return true;
}
} //namespace graphics
} //namespace poke
