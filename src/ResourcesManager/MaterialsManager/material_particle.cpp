#include <ResourcesManager/MaterialsManager/material_particle.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {

MaterialParticle::MaterialParticle(
    std::experimental::optional<const graphics::Image2d&> textureBaseColor)
    : textureBaseColor_(textureBaseColor)
{
    renderMode_ = RenderMode::TRANSPARENT;

    if (textureBaseColor_) {
        const auto diffuse = graphics::MaterialExportData(&*textureBaseColor_);
        descriptorData_.emplace_back(kSamplerColorHash, diffuse);
    }
}

bool MaterialParticle::operator==(const MaterialParticle& other) const
{
	return textureBaseColor_ && other.textureBaseColor_ && &textureBaseColor_.value() == &other.textureBaseColor_.value() || !textureBaseColor_ && !other.textureBaseColor_;
}

bool MaterialParticle::operator!=(const MaterialParticle& other) const
{
	return !(*this == other);
}

void MaterialParticle::CreatePipeline(const graphics::VertexInput& vertexInput)
{
	if (pipelineMaterial_)
		return;

	ResetPipeline();
}

void MaterialParticle::SetTextureBaseColor(const graphics::Image2d& textureBaseColor)
{
	textureBaseColor_ = std::experimental::optional<const graphics::Image2d&>(
		textureBaseColor);

	auto diffuse = graphics::MaterialExportData(&*textureBaseColor_);

	for (auto& data : descriptorData_) {
		if (data.first == kSamplerColorHash) {
			data.second = diffuse;
			return;
		}
	}

	descriptorData_.emplace_back(kSamplerColorHash, diffuse);
	ResetPipeline();
}

void MaterialParticle::ResetTextureBaseColor()
{
	textureBaseColor_.reset();

	for (auto it = descriptorData_.begin(); it != descriptorData_.end(); ++it) {
		if (it->first == kSamplerColorHash) {
			descriptorData_.erase(it);
			break;
		}
	}

	ResetPipeline();
}

json MaterialParticle::ToJson() const
{
	json jsonMaterial;
	jsonMaterial["materialType"] = static_cast<int>(graphics::MaterialType::PARTICLE);

	if (textureBaseColor_) { jsonMaterial["textureBaseColor"] = textureBaseColor_->GetFilename(); }

	return jsonMaterial;
}

void MaterialParticle::SetFromJson(const json& materialJson)
{
	if (CheckJsonExists(materialJson, "textureBaseColor")) {
		//TODO(@Nico) Material can't access TextureManager
		textureBaseColor_.emplace(
			TextureManagerLocator::Get().GetTexture2DByName(
				materialJson["textureBaseColor"]));

		SetTextureBaseColor(*textureBaseColor_);
	}
}

void MaterialParticle::ResetPipeline()
{
	const graphics::Pipeline::Stage stage = renderMode_ == RenderMode::OPAQUE ?
		graphics::Pipeline::Stage{ 0, 0 } :
		graphics::Pipeline::Stage{ 0, 1 };

	pipelineMaterial_ = std::experimental::optional<graphics::PipelineMaterial&>(
		graphics::PipelineMaterial::Create(
			stage,
			{
				"particle",
				{
					graphics::VertexMesh::GetVertexInput(0),
                    graphics::ParticleInstance::Instance::GetVertexInput(1)
				},
				graphics::PipelineGraphics::Mode::MRT,
				graphics::PipelineGraphics::Depth::READ,
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				VK_POLYGON_MODE_FILL,
				VK_CULL_MODE_FRONT_BIT
			}));
}
} //namespace poke
