#include <ResourcesManager/TextureManagers/core_textures_manager.h>

#include <Utility/log.h>
#include <Math/hash.h>

namespace poke {
CoreTexturesManager::CoreTexturesManager()
{
	image2ds_.reserve(kImageDefaultSize);
	image2dIDs_.reserve(kImageDefaultSize);

	imagesCubes_.reserve(kImageCubeDefaultSize);
	imageCubeIDs_.reserve(kImageCubeDefaultSize);
}

void CoreTexturesManager::AddTexture2D(const std::string& texturePath)
{
	const auto resourceID = math::HashString(texturePath);

	image2dIDs_.emplace_back(resourceID);

    image2ds_.emplace_back(
        std::make_unique<graphics::Image2d>(
            texturePath,
            VK_FILTER_LINEAR,
            VK_SAMPLER_ADDRESS_MODE_REPEAT,
            true,
            true,
            false));
	image2ds_.back()->Load();
}

const graphics::Image2d& CoreTexturesManager::GetTexture2DByID(
    const ResourceID resourceID) const
{
    for(size_t i = 0; i < image2ds_.size(); i++) {
        if(image2dIDs_[i] == resourceID) {
			return *image2ds_[i];
        }
    }
	cassert(false, "The image doesn't exist");
}

const graphics::Image2d& CoreTexturesManager::GetTexture2DByName(
    const std::string& texturePath) const
{
	const auto resourceID = math::HashString(texturePath);
	return GetTexture2DByID(resourceID);
}

void CoreTexturesManager::AddTextureCube(const std::string& texturePath)
{
	const auto resourceID = math::HashString(texturePath);

	imageCubeIDs_.emplace_back(resourceID);

	imagesCubes_.emplace_back(
		graphics::ImageCube(
			texturePath,
			".png",
			VK_FILTER_LINEAR,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			true,
			true,
			false));
	imagesCubes_.back().Load();
}

const graphics::ImageCube& CoreTexturesManager::GetTextureCubeByID(
    const ResourceID resourceID
) const
{
    for(size_t i = 0; i < imagesCubes_.size(); i++) {
        if(imageCubeIDs_[i] == resourceID) {
			return imagesCubes_[i];
        }
    }

	cassert(false, "The image cube doesn't exist");
}

const graphics::ImageCube& CoreTexturesManager::GetTextureCubeByName(
    const std::string& texturePath) const
{
	const auto resourceID = math::HashString(texturePath);

    return GetTextureCubeByID(resourceID);
}

void CoreTexturesManager::Clear()
{
    image2ds_.clear();
	image2dIDs_.clear();

    imagesCubes_.clear();
	imageCubeIDs_.clear();
}

void CoreTexturesManager::Resize(const size_t newSize)
{
    if (image2dIDs_.size() > newSize) {
		image2dIDs_.resize(newSize);
		image2ds_.resize(newSize);
    } else {
		imageCubeIDs_.reserve(newSize);
        image2ds_.reserve(newSize);
    }
}

void CoreTexturesManager::SetFromJson(const json& texturesJson)
{
	for (const auto& textureJson : texturesJson) {
		std::string textureName = textureJson;

        //TODO(@Nico) Refactor the scene data to add a separation between 2D and Cube.
		if (textureName.find('.') != std::string::npos) {
			AddTexture2D(textureName);
		}
		else {
			AddTextureCube(textureName);
		}
	}
}

json CoreTexturesManager::ToJson()
{
	LogWarning("You're trying to save using the CoreTexturesManager, nothing will be saved");
    return json();
}
} //namespace poke
