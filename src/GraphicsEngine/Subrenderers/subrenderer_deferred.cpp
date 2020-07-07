#include <GraphicsEngine/Subrenderers/subrenderer_deferred.h>

#include <CoreEngine/engine.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <GraphicsEngine/Pipelines/pipeline_compute.h>
#include <GraphicsEngine/Buffers/push_handle.h>
#include <GraphicsEngine/Lights/light_command_buffer.h>

namespace poke {
namespace graphics {
SubrendererDeferred::SubrendererDeferred(
    const Pipeline::Stage pipelineStage)
    : RenderPipeline(pipelineStage),
      pipeline_(
          pipelineStage,
          "deferred",
          {},
          PipelineGraphics::Mode::MRT,
          PipelineGraphics::Depth::NONE,
          VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
          VK_POLYGON_MODE_FILL,
          0,
          VK_FRONT_FACE_COUNTER_CLOCKWISE,
          false)
{
    /*brdf_ = promiseBrdf_.get_future();
    std::function<void()> func = [this]() { promiseBrdf_.set_value(ComputeBRDF(kBrdfSize_)); };*/

    //GraphicsEngineLocator::Get().GetEngine().AddTask(func);

	currentBrdf_ = ComputeBRDF(kBrdfSize_);

    GraphicsEngineLocator::Get().RegisterObserverUpdateSkybox(
        [this](const ImageCube& skybox) { OnSkyboxUpdate(skybox); });
}

SubrendererDeferred::~SubrendererDeferred()
{
    //std::cout << "~SubrendererDeferred\n";
}

void SubrendererDeferred::OnRender(const CommandBuffer& commandBuffer)
{
    const auto camera = GraphicsEngineLocator::Get().GetCameraData();

    uniformScene_.Push(kViewHash, camera.viewMatrix);
    uniformScene_.Push(kCameraPositionHash, camera.position);

    const auto& lightCommandBuffer = GraphicsEngineLocator::Get().GetLightCommandBuffer();
    //Directional light
    math::Vec3 lightDir = lightCommandBuffer.GetDirectionalCmds().direction;
    lightDir *= -1.0f;
    lightDir = lightDir.Normalize();
    uniformScene_.Push(kDirectionalLightDirectionHash, lightDir);
    uniformScene_.Push(
        kDirectionalLightColorHash,
        lightCommandBuffer.GetDirectionalCmds().color);

    //Lights counts
    uniformScene_.Push(
        kPointLightCountHash,
        static_cast<int>(lightCommandBuffer.GetPointLightDrawCmds().size()));

    uniformScene_.Push(
        kSpotLightCountHash,
        static_cast<int>(lightCommandBuffer.GetSpotLightDrawCmds().size()));

    //Points lights
    storagePointLight_.Push(
        lightCommandBuffer.GetPointLightDrawCmds().data(),
        sizeof(PointLightDrawCommand) * LightCommandBuffer::maxPointLight);
    descriptorSet_.Push(kBufferPointLightsHash, storagePointLight_);


    //Spots lights
    storageSpotLight_.Push(
        lightCommandBuffer.GetSpotLightDrawCmds().data(),
        sizeof(SpotLightDrawCommand) * LightCommandBuffer::maxSpotLight);
    descriptorSet_.Push(kBufferSpotLightsHash, storageSpotLight_);

    //Update descriptor set
    descriptorSet_.Push(kUniformSceneHash, uniformScene_);

   descriptorSet_.Push(
        kSamplerPositionHash,
        GraphicsEngineLocator::Get().GetAttachment("position"));

     descriptorSet_.Push(
        kSamplerDiffuseHash,
        GraphicsEngineLocator::Get().GetAttachment("diffuse"));

    descriptorSet_.Push(
        kSamplerNormalHash,
        GraphicsEngineLocator::Get().GetAttachment("normal"));

    descriptorSet_.Push(
        kSamplerMaterialHash,
        GraphicsEngineLocator::Get().GetAttachment("material"));

    //descriptorSet_.Push(kSamplerBRDFHash, &*currentBrdf_);
    descriptorSet_.Push(kSamplerBRDFHash, currentBrdf_.get());

    //descriptorSet_.Push(kSamplerIrradianceHash, &*currentIrradiance_);
    descriptorSet_.Push(kSamplerIrradianceHash, currentIrradiance_.get());

    //descriptorSet_.Push(kSamplerPrefilteredHash, &*currentPrefiltered_);
    descriptorSet_.Push(kSamplerPrefilteredHash, currentPrefiltered_.get());

    const auto updateSuccess = descriptorSet_.Update(pipeline_);

    if (!updateSuccess) { return; }

    pipeline_.BindPipeline(commandBuffer);

    descriptorSet_.BindDescriptor(commandBuffer, pipeline_);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

std::unique_ptr<Image2d> SubrendererDeferred::ComputeBRDF(uint32_t size)
{
    auto imageBRDF = std::make_unique<Image2d>(
        size,
        size,
        std::vector<char>(),
        VK_FORMAT_R16G16_SFLOAT,
        VK_IMAGE_LAYOUT_GENERAL);

    auto commandBuffer = CommandBuffer(true, VK_QUEUE_COMPUTE_BIT);

    const PipelineCompute computePipeline("brdf");

    computePipeline.BindPipeline(commandBuffer);

    auto descriptorSet = DescriptorHandle(computePipeline);
    descriptorSet.Push(kOutColorHash, imageBRDF.get());
    descriptorSet.Update(computePipeline);

    descriptorSet.BindDescriptor(commandBuffer, computePipeline);
    const auto extent = math::Vec2(
        imageBRDF->GetWidth(),
        imageBRDF->GetHeight());
    computePipeline.Draw(commandBuffer, extent);
    commandBuffer.SubmitIdle();

    return imageBRDF;
}

std::unique_ptr<ImageCube> SubrendererDeferred::ComputeIrradiance(
    const ImageCube& source,
    uint32_t size)
{
    auto irradianceCubemap = std::make_unique<ImageCube>(
        size,
        size,
        std::vector<char>(),
        VK_FORMAT_R32G32B32A32_SFLOAT,
        VK_IMAGE_LAYOUT_GENERAL);

    auto commandBuffer = CommandBuffer(true, VK_QUEUE_COMPUTE_BIT);

    const PipelineCompute computePipeline("irradiance");

    computePipeline.BindPipeline(commandBuffer);

    auto descriptorSet = DescriptorHandle(computePipeline);
    descriptorSet.Push(kOutColorHash, irradianceCubemap.get());
    descriptorSet.Push(kSamplerColorHash, source);
    descriptorSet.Update(computePipeline);

    descriptorSet.BindDescriptor(commandBuffer, computePipeline);
    const auto extent = math::Vec2(
        irradianceCubemap->GetWidth(),
        irradianceCubemap->GetHeight());

    computePipeline.Draw(commandBuffer, extent);
    commandBuffer.SubmitIdle();

    return irradianceCubemap;
}

std::unique_ptr<ImageCube> SubrendererDeferred::ComputePrefiltered(
    const ImageCube& source,
    uint32_t size)
{
    const auto& logicalDevice = GraphicsEngineLocator::Get().GetLogicalDevice();

    auto prefilteredCubemap = std::make_unique<ImageCube>(
        size,
        size,
        std::vector<char>(),
        VK_FORMAT_R16G16B16A16_SFLOAT,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
        VK_FILTER_LINEAR,
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        VK_SAMPLE_COUNT_1_BIT,
        true,
        true);

    auto commandBuffer = CommandBuffer(true, VK_QUEUE_COMPUTE_BIT);

    const PipelineCompute computePipeline("prefiltered", false);

    auto descriptorSet = DescriptorHandle(computePipeline);
    auto pushHandle = PushHandle(computePipeline.GetShader().GetUniformBlock(kPushObjectHash));

    for (uint32_t i = 0; i < prefilteredCubemap->GetMipLevels(); i++) {
        commandBuffer.Begin();

        const VkImageView levelView = Image::CreateImageView(
            prefilteredCubemap->GetImage(),
            VK_IMAGE_VIEW_TYPE_CUBE,
            prefilteredCubemap->GetFormat(),
            VK_IMAGE_ASPECT_COLOR_BIT,
            1,
            i,
            6,
            0);

        commandBuffer.Begin();
        computePipeline.BindPipeline(commandBuffer);

        const VkDescriptorImageInfo imageInfo{
            prefilteredCubemap->GetSampler(),
            levelView,
            prefilteredCubemap->GetLayout()
        };

        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = VK_NULL_HANDLE;
        descriptorWrite.dstBinding = computePipeline.GetShader().GetDescriptorLocation(
            kOutColorHash);
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.descriptorType =
            computePipeline.GetShader().GetDescriptorType(
                descriptorWrite.
                dstBinding);

        auto writeDescriptorSet = WriteDescriptorSet(descriptorWrite, imageInfo);

        pushHandle.Push(
            kRoughnessHash,
            static_cast<float>(i) / static_cast<float>(
                prefilteredCubemap->GetMipLevels() - 1));

        descriptorSet.Push(kPushObjectHash, pushHandle);

        descriptorSet.Push(
            kOutColorHash,
            prefilteredCubemap.get(),
            std::move(writeDescriptorSet));

        descriptorSet.Push(kSamplerColorHash, source);
        descriptorSet.Update(computePipeline);

        descriptorSet.BindDescriptor(commandBuffer, computePipeline);
        pushHandle.BindPush(commandBuffer, computePipeline);
        computePipeline.Draw(
            commandBuffer,
            math::Vec2(
                prefilteredCubemap->GetWidth() << 1,
                prefilteredCubemap->GetWidth() << 1));
        commandBuffer.SubmitIdle();

        vkDestroyImageView(logicalDevice, levelView, nullptr);
    }

    return prefilteredCubemap;
}

void SubrendererDeferred::OnSkyboxUpdate(const ImageCube& skybox)
{
	currentIrradiance_ = ComputeIrradiance(skybox, kIrradianceSize_);
	currentPrefiltered_ = ComputePrefiltered(skybox, kPrefilteredSize_);

    //static bool loaded = false;
    //if (loaded)
    //    return;
    //loaded = true;
    //if (irradiance_.HasValue()) { irradiance_ = Future<std::unique_ptr<ImageCube>>(); }

    //irradiance_ = promiseIrradiance_.get_future();
    //std::function<void()> func1 = [&]() {
    //    promiseIrradiance_.set_value(ComputeIrradiance(skybox, kIrradianceSize_));
    //    promiseIrradiance_ = std::promise<std::unique_ptr<ImageCube>>();
    //};
    //GraphicsEngineLocator::Get().GetEngine().AddTask(func1);

    //if (prefiltered_.HasValue()) { prefiltered_ = Future<std::unique_ptr<ImageCube>>(); }

    //prefiltered_ = promisePrefiltered_.get_future();
    //std::function<void()> func2 = [&]() {
    //    promisePrefiltered_.set_value(ComputePrefiltered(skybox, kPrefilteredSize_));
    //    promisePrefiltered_ = std::promise<std::unique_ptr<ImageCube>>();
    //};
    //GraphicsEngineLocator::Get().GetEngine().AddTask(func2);
}
} //namespace graphics
} //namespace poke
