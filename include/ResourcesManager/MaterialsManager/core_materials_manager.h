//----------------------------------------------------------------------------------
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
// Date : 12.02.20
//----------------------------------------------------------------------------------
#pragma once
#include <vector>

#include <ResourcesManager/MaterialsManager/interface_materials_manager.h>
#include <ResourcesManager/MaterialsManager/material_skybox.h>
#include <ResourcesManager/MaterialsManager/material_diffuse.h>
#include <ResourcesManager/MaterialsManager/material_trail.h>
#include <ResourcesManager/MaterialsManager/material_particle.h>

namespace poke {
class CoreMaterialsManager : public IMaterialsManager {
public:
    CoreMaterialsManager();
    ~CoreMaterialsManager() = default;

    void AddMaterial(const std::string& materialName) override;

    graphics::Material& GetMaterial(const std::string& materialName) override;
    graphics::Material& GetMaterial(ResourceID resourceID) override;

    void Clear() override;

    void Resize(size_t newSize) override;

    ResourceID GetDefaultMaterialID() override;

    ResourceID GetDefaultSkyboxMaterialID() override;

    void SetFromJson(const json& materialsJson) override;

    json ToJson() override;

protected:
    std::vector<ResourceID> skyboxMaterialIDs_;
    std::vector<MaterialSkybox> skyboxMaterials_;

    std::vector<ResourceID> diffuseMaterialIDs_;
    std::vector<MaterialDiffuse> diffuseMaterials_;

    std::vector<ResourceID> trailMaterialIDs_;
    std::vector<MaterialTrail> trailMaterials_;

	std::vector<ResourceID> particleMaterialIDs_;
	std::vector<MaterialParticle> particleMaterials_;

    inline static const size_t kMaterialsDefaultSize = 100;
    inline static const size_t kMaterialsSkyboxDefaultSize = 2;

    //TODO(@Nico) Add default texture inside texture manager
    graphics::ImageCube defaultImageCube_ = graphics::ImageCube(
        "skybox",
        ".png",
        VK_FILTER_LINEAR,
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        true,
        true,
        false);
    inline static const std::string kDefaultMaterialName =
        "default_material_name";

    inline static const std::string kDefaultSkyboxMaterialName =
        "default_skybox_material_name";

    bool defaultMaterialIDLoaded_ = false;
    bool defaultMaterialSkyboxIDLoaded_ = false;
};
} //namespace poke
