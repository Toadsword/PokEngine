#include <ResourcesManager/MaterialsManager/null_materials_manager.h>

#include <CoreEngine/cassert.h>

namespace poke {

void NullMaterialsManager::AddMaterial(const std::string& materialName)
{
	
}

graphics::Material& NullMaterialsManager::
GetMaterial(const std::string& materialName)
{
    cassert(false, "Impossible to acces to null material manager");
	abort();
}

graphics::Material& NullMaterialsManager::
GetMaterial(ResourceID resourceID)
{
    cassert(false, "Impossible to acces to null material manager");
	abort();
}

XXH64_hash_t NullMaterialsManager::GetDefaultMaterialID()
{
	return 0;
}

XXH64_hash_t NullMaterialsManager::GetDefaultSkyboxMaterialID()
{
	return 0;
}
} //namespace poke
