#include <ResourcesManager/MaterialsManager/material_skybox.h>

#include <GraphicsEngine/Buffers/uniform_handle.h>
#include <Ecs/ComponentManagers/transforms_manager.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <json.hpp>

namespace poke {
MaterialSkybox::MaterialSkybox(
    const std::experimental::optional<const graphics::ImageCube&> texture,
    const Color colorDiffuse)
    : texture_(texture),
      colorDiffuse_(colorDiffuse),
      blend_(1.0f)
{
    if (texture_) {
        const auto diffuse = graphics::MaterialExportData(&*texture_);
        descriptorData_.emplace_back(kSamplerColorHash, diffuse);
    }

    const auto baseDiffuse = graphics::MaterialExportData(colorDiffuse_);
    uniformData_.emplace_back(kBaseColorHash, baseDiffuse);


    const auto blendData = graphics::MaterialExportData(blend_);
    uniformData_.emplace_back(kBlendFactorHash, blendData);
}

bool MaterialSkybox::operator==(const MaterialSkybox& other) const
{
    return ((texture_ && other.texture_ && &texture_.value() == &other.texture_.value()) || !texture_ && !other.texture_)
           && colorDiffuse_ == other.colorDiffuse_
           && blend_ == other.blend_;
}

bool MaterialSkybox::operator!=(const MaterialSkybox& other) const { return !(*this == other); }

void MaterialSkybox::CreatePipeline(const graphics::VertexInput& vertexInput)
{
    if (pipelineMaterial_)
        return;

    //TODO(@Nico) find a way to set in an automatic way the pipeline::stage
    pipelineMaterial_ = std::experimental::optional<graphics::PipelineMaterial&>(
        graphics::PipelineMaterial::Create(
            {0, 0},
            {
                "skybox",
                {vertexInput},
                graphics::PipelineGraphics::Mode::MRT,
                graphics::PipelineGraphics::Depth::NONE,
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                VK_POLYGON_MODE_FILL,
                VK_CULL_MODE_FRONT_BIT,
                VK_FRONT_FACE_COUNTER_CLOCKWISE
            }));
}

void MaterialSkybox::SetTexture(const graphics::ImageCube& texture)
{
    texture_ = std::experimental::optional<const graphics::ImageCube&>(texture);

    auto diffuse = graphics::MaterialExportData(&*texture_);

    for (auto& data : descriptorData_) {
        if (data.first == kSamplerColorHash) {
            data.second = diffuse;
            return;
        }
    }

    descriptorData_.emplace_back(kSamplerColorHash, diffuse);
}

void MaterialSkybox::SetColorDiffuse(const Color colorDiffuse)
{
	colorDiffuse_ = colorDiffuse;

    const auto colorData = graphics::MaterialExportData(colorDiffuse_);

    for (auto& data : uniformData_) {
        if(data.first == kBaseColorHash) {
			data.second = colorData;
			return;
        }
    }

    uniformData_.emplace_back(kBaseColorHash, colorData);
}

void MaterialSkybox::SetBlend(const float blend) { 
	blend_ = blend;

	const auto blendData = graphics::MaterialExportData(blend_);

	for (auto& data : uniformData_) {
		if (data.first == kBlendFactorHash) {
			data.second = blendData;
			return;
		}
	}

	uniformData_.emplace_back(kBlendFactorHash, blendData);
}

json MaterialSkybox::ToJson() const
{
    json jsonMaterial;
    jsonMaterial["materialType"] = static_cast<int>(graphics::MaterialType::
        SKYBOX);
    jsonMaterial["colorDiffuse"] = colorDiffuse_.ToJson();

    if (texture_) { jsonMaterial["texture"] = texture_->GetFilename(); }

    jsonMaterial["blend"] = blend_;

    return jsonMaterial;
}

void MaterialSkybox::SetFromJson(const json& materialJson)
{
	Color color;
	color.SetFromJson(materialJson["colorDiffuse"]);
	SetColorDiffuse(color);

    if (CheckJsonExists(materialJson, "texture")) {
        texture_.emplace(
            TextureManagerLocator::Get().GetTextureCubeByName(
                materialJson["texture"]));

		SetTexture(*texture_);
    }

	SetBlend(materialJson["blend"]);
}

graphics::MaterialType MaterialSkybox::GetType() const { return graphics::MaterialType::SKYBOX; }
} // namespace poke
