//-----------------------------------------------------------------------------
// Copyright (c) 2019-2020, POK Family. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of POK Family nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author : Nicolas Schneider
// Co-Author :
// Date : 29.01.20
//-----------------------------------------------------------------------------
#pragma once

#include <GraphicsEngine/Pipelines/render_pipeline.h>
#include <GraphicsEngine/Descriptors/descriptor_handle.h>
#include <GraphicsEngine/Pipelines/pipeline_graphic.h>
#include <GraphicsEngine/Buffers/uniform_handle.h>
#include <GraphicsEngine/Buffers/storage_handle.h>
#include <math/hash.h>
#include <Utility/future.h>

namespace poke {
namespace graphics {
class Image2d;
class ImageCube;

class SubrendererDeferred : public RenderPipeline {
public:
    explicit SubrendererDeferred(
        Pipeline::Stage pipelineStage);

    ~SubrendererDeferred();

    void OnRender(const CommandBuffer& commandBuffer) override;

    static std::unique_ptr<Image2d> ComputeBRDF(
        uint32_t size);

    static std::unique_ptr<ImageCube> ComputeIrradiance(
        const ImageCube& source,
        uint32_t size);

    static std::unique_ptr<ImageCube> ComputePrefiltered(
        const ImageCube& source,
        uint32_t size);

	constexpr static int GetSubrendererIndex()
	{
		return static_cast<int>(SubrendererIndex::DEFERRED);
	}
private:

    void OnSkyboxUpdate(const ImageCube& skybox);

    PipelineGraphics pipeline_;
    DescriptorHandle descriptorSet_;
    UniformHandle uniformScene_;

    //BRDF
	std::unique_ptr<Image2d> currentBrdf_;
	/*Future<std::unique_ptr<Image2d>> brdf_;
	std::promise<std::unique_ptr<Image2d>> promiseBrdf_;*/
    const int kBrdfSize_ = 512;

    //Irradiance
	std::unique_ptr<ImageCube> currentIrradiance_;
	/*Future<std::unique_ptr<ImageCube>> irradiance_;
	std::promise<std::unique_ptr<ImageCube>> promiseIrradiance_;*/
    const int kIrradianceSize_ = 64;

    //Prefiltered
	std::unique_ptr<ImageCube> currentPrefiltered_;
	//Future<std::unique_ptr<ImageCube>> prefiltered_;
	//std::promise<std::unique_ptr<ImageCube>> promisePrefiltered_;
    const int kPrefilteredSize_ = 512;

    //Lights
    StorageHandle storagePointLight_;
    StorageHandle storageSpotLight_;

    //Hash
    inline static const math::StringHash kViewHash = math::HashString("view");

    inline static const math::StringHash kCameraPositionHash = 
		math::HashString("cameraPosition");

    inline static const math::StringHash kDirectionalLightDirectionHash = 
		math::HashString("directionalLightDirection");

    inline static const math::StringHash kDirectionalLightColorHash = 
		math::HashString("directionalLightColor");

    inline static const math::StringHash kPointLightCountHash = 
		math::HashString("pointLightCount");

    inline static const math::StringHash kSpotLightCountHash = 
		math::HashString("spotLightCount");

    inline static const math::StringHash kBufferPointLightsHash =
		math::HashString("BufferPointLights");

    inline static const math::StringHash kBufferSpotLightsHash = 
		math::HashString("BufferSpotLights");

    inline static const math::StringHash kUniformSceneHash = 
		math::HashString("UniformScene");

    inline static const math::StringHash kSamplerPositionHash = 
		math::HashString("samplerPosition");

    inline static const math::StringHash kSamplerDiffuseHash = 
		math::HashString("samplerDiffuse");

    inline static const math::StringHash kSamplerNormalHash = 
		math::HashString("samplerNormal");

    inline static const math::StringHash kSamplerMaterialHash = 
		math::HashString("samplerMaterial");

    inline static const math::StringHash kSamplerBRDFHash = 
		math::HashString("samplerBRDF");

    inline static const math::StringHash kSamplerIrradianceHash = 
		math::HashString("samplerIrradiance");

    inline static const math::StringHash kSamplerPrefilteredHash = 
		math::HashString("samplerPrefiltered");

    inline static const math::StringHash kOutColorHash = 
		math::HashString("outColor");

    inline static const math::StringHash kSamplerColorHash = 
		math::HashString("samplerColor");

    inline static const math::StringHash kRoughnessHash = 
		math::HashString("roughness");

    inline static const math::StringHash kPushObjectHash = 
		math::HashString("PushObject");
};
} //namespace graphics
} //namespace poke
