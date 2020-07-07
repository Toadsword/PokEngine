#include <Ecs/Prefabs/engine_prefab.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace ecs {
const Archetype& EnginePrefab::GetObject(const size_t index) const
{
    return archetypes_[index];
}

std::vector<EntityIndex> EnginePrefab::Instantiate() const
{
    return EcsManagerLocator::Get().InstantiatePrefab(*this);
}

void EnginePrefab::SetFromJson(const json& prefabJson)
{
    names_.resize(prefabJson.size());
    archetypes_.resize(prefabJson.size());
    archetypesID_.resize(prefabJson.size());
    parentIndexes_.resize(prefabJson.size());

    for (size_t i = 0; i < prefabJson.size(); i++) {
        archetypes_[i].SetFromJson(prefabJson[i]);

        if (CheckJsonExists(prefabJson[i], "archetypeID")) {
            archetypesID_[i] = prefabJson[i]["archetypeID"];
        }

        parentIndexes_[i] = prefabJson[i]["transform"]["parent"];
		
        names_[i] = prefabJson[i]["objectName"].get<std::string>();
    }
}

json EnginePrefab::ToJson() const
{
    json prefabJson;

    for (size_t i = 0; i < names_.size(); i++) {
        prefabJson[i] = archetypes_[i].ToJson();

        if (archetypesID_[i] != 0) {
            prefabJson[i]["archetypeID"] = archetypesID_[i];
        }

        prefabJson[i]["transform"]["parent"] = parentIndexes_[i];
        prefabJson[i]["objectName"] = names_[i];
    }

    return prefabJson;
}
} //namespace ecs
} //namespace poke
