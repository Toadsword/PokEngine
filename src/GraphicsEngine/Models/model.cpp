#include <GraphicsEngine/Models/model.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace graphics {
Model::Model(
    const XXH64_hash_t material,
    const XXH64_hash_t mesh)
    : materialID(material),
      meshID(mesh) {}

void Model::SetFromJson(json jsonModel)
{
    materialID = jsonModel["materialHash"];
    meshID = jsonModel["meshHash"];
}

json Model::ToJson() const
{
    json modelJson;
    modelJson["materialHash"] = materialID;
    modelJson["meshHash"] = meshID;
    return modelJson;
}

bool Model::operator==(const Model& other) const
{
	return materialID == other.materialID
		&& meshID == other.meshID;
}

bool Model::operator!=(const Model& other) const { return !(*this == other); }
} //namespace graphics
} //namespace poke
