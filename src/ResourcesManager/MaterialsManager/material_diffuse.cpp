#include <ResourcesManager/MaterialsManager/material_diffuse.h>

#include <GraphicsEngine/graphic_engine.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <json.hpp>

namespace poke {
MaterialDiffuse::MaterialDiffuse(
    Color color,
    const float metallic,
    const float roughness,
    std::experimental::optional<const graphics::Image2d&> textureBaseColor,
    std::experimental::optional<const graphics::Image2d&> textureRMA,
    std::experimental::optional<const graphics::Image2d&> textureNormal)
    : color_(color),
      metallic_(metallic),
      roughness_(roughness),
      textureBaseColor_(textureBaseColor),
      textureRMA_(textureRMA),
      textureNormal_(textureNormal)
{
    if (textureBaseColor_) {
        const auto diffuse = graphics::MaterialExportData(&*textureBaseColor_);
        descriptorData_.emplace_back(kSamplerBaseColorHash, diffuse);
    }

    if (textureRMA_) {
        const auto mra = graphics::MaterialExportData(&*textureRMA_);
        descriptorData_.emplace_back(kSamplerMaterialHash, mra);
    }

    if (textureNormal_) {
        const auto normal = graphics::MaterialExportData(&*textureNormal_);
        descriptorData_.emplace_back(kSamplerNormalHash, normal);
    }


    const auto baseDiffuse = graphics::MaterialExportData(color_);
    uniformData_.emplace_back(kColorHash, baseDiffuse);


    const auto metallicData = graphics::MaterialExportData(metallic_);
    uniformData_.emplace_back(kMetallicHash, metallicData);


    const auto roughnessData = graphics::MaterialExportData(roughness_);
    uniformData_.emplace_back(kRoughnessHash, roughnessData);
}

bool MaterialDiffuse::operator==(const MaterialDiffuse& other) const
{
	return shaderName_ == other.shaderName_
		&& color_ == other.color_
		&& metallic_ == other.metallic_
		&& roughness_ == other.roughness_
		&& (textureBaseColor_ && other.textureBaseColor_ && &textureBaseColor_.value() == &other.textureBaseColor_.value() || !textureBaseColor_ && !other.textureBaseColor_)
		&& (textureNormal_ && other.textureNormal_ && &textureNormal_.value() == &other.textureNormal_.value() || !textureNormal_ && !other.textureNormal_)
		&& (textureRMA_ && other.textureRMA_ && &textureRMA_.value() == &other.textureRMA_.value() || !textureRMA_ && !other.textureRMA_);
}

bool MaterialDiffuse::operator!=(const MaterialDiffuse& other) const { return !(*this == other); }

void MaterialDiffuse::CreatePipeline(const graphics::VertexInput& vertexInput)
{
    if (pipelineMaterial_)
        return;

    shaderName_ = GetShaderName();

    ResetPipeline();
}

void MaterialDiffuse::SetColor(const Color& color)
{
    color_ = color;

    auto baseDiffuseData = graphics::MaterialExportData(color_);

    for (auto& data : uniformData_) {
        if (data.first == kColorHash) {
            data.second = baseDiffuseData;
            return;
        }
    }

    uniformData_.emplace_back(kColorHash, baseDiffuseData);
}

std::experimental::optional<const graphics::Image2d&> MaterialDiffuse::GetTextureBaseColor() const
{
    return textureBaseColor_;
}

void MaterialDiffuse::SetTextureBaseColor(const graphics::Image2d& textureBaseColor)
{
    textureBaseColor_ = std::experimental::optional<const graphics::Image2d&>(
        textureBaseColor);

    auto diffuse = graphics::MaterialExportData(&*textureBaseColor_);

    for (auto& data : descriptorData_) {
        if (data.first == kSamplerBaseColorHash) {
            data.second = diffuse;
            return;
        }
    }

    descriptorData_.emplace_back(kSamplerBaseColorHash, diffuse);
	shaderName_ = GetShaderName();
	ResetPipeline();
}

void MaterialDiffuse::ResetTextureBaseColor()
{
    textureBaseColor_.reset();

    for (auto it = descriptorData_.begin(); it != descriptorData_.end(); ++it) {
        if (it->first == kSamplerBaseColorHash) {
            descriptorData_.erase(it);
            break;
        }
    }

    shaderName_ = GetShaderName();
    ResetPipeline();
}

void MaterialDiffuse::SetMetallic(const float metallic)
{
    metallic_ = metallic;

    for (auto& data : uniformData_) {
        if (data.first == kMetallicHash) {
            data.second = graphics::MaterialExportData(metallic_);
            return;
        }
    }
}

void MaterialDiffuse::SetRoughness(const float roughness)
{
    roughness_ = roughness;

    for (auto& data : uniformData_) {
        if (data.first == kRoughnessHash) {
            data.second = graphics::MaterialExportData(roughness_);
            return;
        }
    }
}

void MaterialDiffuse::SetTextureRMA(const graphics::Image2d& textureRMA)
{
    textureRMA_ = std::experimental::optional<const graphics::Image2d&>(textureRMA);

    auto mra = graphics::MaterialExportData(&*textureRMA_);

    for (auto& data : descriptorData_) {
        if (data.first == kSamplerMaterialHash) {
            data.second = mra;
            return;
        }
    }

    descriptorData_.emplace_back(kSamplerMaterialHash, mra);
	shaderName_ = GetShaderName();
	ResetPipeline();
}

void MaterialDiffuse::ResetTextureRMA()
{
    textureRMA_.reset();

    for (auto it = descriptorData_.begin(); it != descriptorData_.end(); ++it
    ) {
        if (it->first == kSamplerMaterialHash) {
            descriptorData_.erase(it);
            break;
        }
    }

    shaderName_ = GetShaderName();
	ResetPipeline();
}

void MaterialDiffuse::SetTextureNormal(const graphics::Image2d& textureNormal)
{
    textureNormal_ = std::experimental::optional<const graphics::Image2d&>(textureNormal);

    auto normal = graphics::MaterialExportData(&*textureNormal_);

    for (auto& data : descriptorData_) {
        if (data.first == kSamplerNormalHash) {
            data.second = normal;
            return;
        }
    }

    descriptorData_.emplace_back(kSamplerNormalHash, normal);
	shaderName_ = GetShaderName();
	ResetPipeline();
}

void MaterialDiffuse::ResetTextureNormal()
{
    textureNormal_.reset();

    for (auto it = descriptorData_.begin(); it != descriptorData_.end(); ++it
    ) {
        if (it->first == kSamplerNormalHash) {
            descriptorData_.erase(it);
            break;
        }
    }

    shaderName_ = GetShaderName();
    ResetPipeline();
}

graphics::MaterialType MaterialDiffuse::GetType() const { return graphics::MaterialType::DIFFUSE; }

void MaterialDiffuse::SetRenderMode(const RenderMode renderMode)
{
    renderMode_ = renderMode;
	shaderName_ = GetShaderName();
	ResetPipeline();
}

json MaterialDiffuse::ToJson() const
{
    json jsonMaterial;
    jsonMaterial["materialType"] = static_cast<int>(graphics::MaterialType::DIFFUSE);
    jsonMaterial["renderMode"] = static_cast<int>(renderMode_);
    jsonMaterial["color"] = color_.ToJson();
    jsonMaterial["metallic"] = metallic_;
    jsonMaterial["roughness"] = roughness_;

    if (textureBaseColor_) { jsonMaterial["textureBaseColor"] = textureBaseColor_->GetFilename(); }

    if (textureRMA_) { jsonMaterial["textureRMA"] = textureRMA_->GetFilename(); }

    if (textureNormal_) { jsonMaterial["textureNormal"] = textureNormal_->GetFilename(); }

    return jsonMaterial;
}

void MaterialDiffuse::SetFromJson(const json& materialJson)
{
    //Color
	Color color;
    color.SetFromJson(materialJson["color"]);
	SetColor(color);

    //Metallic
	SetMetallic(materialJson["metallic"]);

    //Roughness
	SetRoughness(materialJson["roughness"]);

	renderMode_ = materialJson["renderMode"];

    if (CheckJsonExists(materialJson, "textureBaseColor")) {
        //TODO(@Nico) Material can't access TextureManager
        textureBaseColor_.emplace(
            TextureManagerLocator::Get().GetTexture2DByName(
                materialJson["textureBaseColor"]));

		SetTextureBaseColor(*textureBaseColor_);
    }

    if (CheckJsonExists(materialJson, "textureRMA")) {
        //TODO(@Nico) Material can't access TextureManager
        textureRMA_.emplace(
            TextureManagerLocator::Get().GetTexture2DByName(
                materialJson["textureRMA"]));

		SetTextureRMA(*textureRMA_);
    }

    if (CheckJsonExists(materialJson, "textureNormal")) {
        textureNormal_.emplace(
            TextureManagerLocator::Get().GetTexture2DByName(
                materialJson["textureNormal"]));

		SetTextureNormal(*textureNormal_);
    }
}

const std::string MaterialDiffuse::GetShaderName() const
{
    std::string shaderName = "diffuse";

	if (renderMode_ == RenderMode::TRANSPARENT) { shaderName += "_t"; }

    if (textureBaseColor_) { shaderName += "_bc"; }

    if (textureNormal_) { shaderName += "_n"; }

    if (textureRMA_) { shaderName += "_rma"; }

    return shaderName;
}

void MaterialDiffuse::ResetPipeline()
{
    //TODO(@Nico) find a way to set in an automatic way the pipeline::stage

	const VkCullModeFlags cullMode = 
		renderMode_ == RenderMode::OPAQUE ? 
		    VK_CULL_MODE_FRONT_BIT :
            static_cast<VkCullModeFlags>(0);

	const graphics::Pipeline::Stage stage = renderMode_ == RenderMode::OPAQUE ? 
		graphics::Pipeline::Stage{0, 0} :
		graphics::Pipeline::Stage{0, 1};

    pipelineMaterial_ = std::experimental::optional<graphics::PipelineMaterial&>(
        graphics::PipelineMaterial::Create(
			stage,
            {
                shaderName_,
                {
                    graphics::VertexMesh::GetVertexInput(0),
                    graphics::ModelInstance::Instance::GetVertexInput(1)
                },
                graphics::PipelineGraphics::Mode::MRT,
                graphics::PipelineGraphics::Depth::READ_WRITE,
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                VK_POLYGON_MODE_FILL,
				cullMode,
                VK_FRONT_FACE_CLOCKWISE
            }));
}
} //namespace poke
