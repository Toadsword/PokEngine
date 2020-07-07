#include <ResourcesManager/MaterialsManager/material_trail.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {

MaterialTrail::MaterialTrail(
    const Color color,
    const std::experimental::optional<const graphics::Image2d&> textureBaseColor) :
    color_(color),
    textureBaseColor_(textureBaseColor)
{
	renderMode_ = RenderMode::TRANSPARENT;

	if (textureBaseColor_) {
		const auto diffuse = graphics::MaterialExportData(&*textureBaseColor_);
		descriptorData_.emplace_back(kSamplerBaseColorHash, diffuse);
	}

	const auto baseDiffuse = graphics::MaterialExportData(color_);
	uniformData_.emplace_back(kColorHash, baseDiffuse);
}

bool MaterialTrail::operator==(const MaterialTrail& other) const
{
	return color_ == other.color_ &&
		(textureBaseColor_ && other.textureBaseColor_ && &textureBaseColor_.value() == &other.textureBaseColor_.value() || !textureBaseColor_ && !other.textureBaseColor_);
}

bool MaterialTrail::operator!=(const MaterialTrail& other) const
{
	return !(*this == other);
}

void MaterialTrail::CreatePipeline(const graphics::VertexInput& vertexInput)
{
	if (pipelineMaterial_)
		return;

	ResetPipeline();
}

void MaterialTrail::SetColor(const Color& color)
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

std::experimental::optional<const graphics::Image2d&> MaterialTrail::GetTextureBaseColor() const
{
	return textureBaseColor_;
}

void MaterialTrail::SetTextureBaseColor(const graphics::Image2d& textureBaseColor)
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
	ResetPipeline();
}

void MaterialTrail::ResetTextureBaseColor()
{
	textureBaseColor_.reset();

	for (auto it = descriptorData_.begin(); it != descriptorData_.end(); ++it) {
		if (it->first == kSamplerBaseColorHash) {
			descriptorData_.erase(it);
			break;
		}
	}

	ResetPipeline();
}

graphics::MaterialType MaterialTrail::GetType() const
{
	return graphics::MaterialType::TRAIL;
}

void MaterialTrail::SetRenderMode(const RenderMode renderMode)
{
	renderMode_ = renderMode;
	ResetPipeline();
}

json MaterialTrail::ToJson() const
{
	json jsonMaterial;
	jsonMaterial["materialType"] = static_cast<int>(graphics::MaterialType::DIFFUSE);
	jsonMaterial["color"] = color_.ToJson();

	if (textureBaseColor_) { jsonMaterial["textureBaseColor"] = textureBaseColor_->GetFilename(); }

	return jsonMaterial;
}

void MaterialTrail::SetFromJson(const json& materialJson)
{
	//Color
	Color color;
	color.SetFromJson(materialJson["color"]);
	SetColor(color);

	if (CheckJsonExists(materialJson, "textureBaseColor")) {
		//TODO(@Nico) Material can't access TextureManager
		textureBaseColor_.emplace(
			TextureManagerLocator::Get().GetTexture2DByName(
				materialJson["textureBaseColor"]));

		SetTextureBaseColor(*textureBaseColor_);
	}
}

void MaterialTrail::ResetPipeline()
{
	const VkCullModeFlags cullMode =
		renderMode_ == RenderMode::OPAQUE ?
		VK_CULL_MODE_BACK_BIT :
		static_cast<VkCullModeFlags>(0);

	const graphics::Pipeline::Stage stage = renderMode_ == RenderMode::OPAQUE ?
		graphics::Pipeline::Stage{ 0, 0 } :
		graphics::Pipeline::Stage{ 0, 1 };

	pipelineMaterial_ = std::experimental::optional<graphics::PipelineMaterial&>(
		graphics::PipelineMaterial::Create(
			stage,
			{
				"trail",
				{
					graphics::VertexMesh::GetVertexInput(0)
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