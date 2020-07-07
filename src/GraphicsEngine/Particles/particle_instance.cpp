#include <GraphicsEngine/Particles/particle_instance.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Math/matrix.h>

namespace poke {
namespace graphics {
VertexInput ParticleInstance::Instance::GetVertexInput(const uint32_t baseBinding)
{
    const std::vector<VkVertexInputBindingDescription> bindingDescriptions = {
        {baseBinding, sizeof(Instance), VK_VERTEX_INPUT_RATE_INSTANCE}
    };
    const std::vector<VkVertexInputAttributeDescription> attributeDescriptions
        = {
            {0, baseBinding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Instance, modelMatrix)},
            {1, baseBinding, VK_FORMAT_R32G32B32A32_SFLOAT,offsetof(Instance, modelMatrix) + 16},
            {2, baseBinding, VK_FORMAT_R32G32B32A32_SFLOAT,offsetof(Instance, modelMatrix) + 32},
            {3, baseBinding, VK_FORMAT_R32G32B32A32_SFLOAT,offsetof(Instance, modelMatrix) + 48},
            {4, baseBinding, VK_FORMAT_R32G32B32A32_SFLOAT,offsetof(Instance, colorOffset)},
            {5, baseBinding, VK_FORMAT_R32G32B32A32_SFLOAT,offsetof(Instance, offset)},
            {6, baseBinding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Instance, blend)}
        };
    return VertexInput(0, bindingDescriptions, attributeDescriptions);
}

ParticleInstance::ParticleInstance(const Material& material)
    : kMesh_(MeshManagerLocator::Get().GetQuad()),
      kMaterial_(material),
      instanceBuffer_(sizeof(Instance) * kMaxInstances) { }

void ParticleInstance::Update(const std::vector<ParticleDrawInfo>& particles)
{
    if (particles.empty()) { return; }

    Instance* instances;
    instanceBuffer_.MapMemory(reinterpret_cast<char**>(&instances));

	auto viewMatrix = CameraLocator::Get().GetViewMatrix();

    for (const auto& particle : particles) {
        if (instances_ >= kMaxInstances) { break; }

        auto instance = &instances[instances_];

		instance->modelMatrix = math::Matrix4::Translate(math::Matrix4::Identity(), particle.position);

		for (uint32_t row = 0; row < 3; row++) {
			for (uint32_t col = 0; col < 3; col++) {
				instance->modelMatrix[row][col] = viewMatrix[col][row];
			}
		}

		instance->modelMatrix.Rotate(0, {0, 0, 1});

        instance->modelMatrix = math::Matrix4::Scale(
            instance->modelMatrix,
            math::Vec3(particle.scale));

        instance->colorOffset = {
            particle.colorOffset.r,
            particle.colorOffset.g,
            particle.colorOffset.b,
            1
        };
        instance->offset = {
            particle.imageOffset1.x,
            particle.imageOffset1.y,
            particle.imageOffset2.x,
            particle.imageOffset2.y
        };
        instance->blend = {
            particle.imageBlendFactor,
            particle.transparency,
            static_cast<float>(1) //TODO(@Nico) Remove the one and use particle.numberOfRows
        };
        instances_++;
    }

    instanceBuffer_.UnmapMemory();
}

bool ParticleInstance::CmdRender(
    const CommandBuffer& commandBuffer,
    UniformHandle& uniformScene)
{
    if (instances_ == 0) { return false; }

    // Check if we are in the correct pipeline stage.
    const auto& materialPipeline = kMaterial_.GetPipelineMaterial();

    // Binds the material pipeline.
    if (!kMaterial_.BindPipeline(commandBuffer))
        return false;

    const auto& pipeline = materialPipeline.GetPipeline();

    // Updates descriptors.
    descriptorSet_.Push(kUniformSceneHash, uniformScene);
	descriptorSet_.Push(math::HashString("samplerDepth"), GraphicsEngineLocator::Get().GetAttachment("depth"));

    descriptorSet_.PushDescriptorData(kMaterial_.ExportDescriptorData());

    if (!descriptorSet_.Update(pipeline)) { return false; }

    // Draws the instanced objects.
    descriptorSet_.BindDescriptor(commandBuffer, pipeline);

    VkBuffer vertexBuffers[2] = {
        kMesh_.GetVertexBuffer().GetBuffer(),
        instanceBuffer_.GetBuffer()
    };
    VkDeviceSize offsets[2] = {0, 0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 2, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(
        commandBuffer,
        kMesh_.GetIndexBuffer().GetBuffer(),
        0,
        kMesh_.GetIndexType());
    vkCmdDrawIndexed(commandBuffer, kMesh_.GetIndexCount(), instances_, 0, 0, 0);
    instances_ = 0;
    return true;
}
} //namespace graphics
} //namespace poke
