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
// Date : 03.03.2020
//----------------------------------------------------------------------------------
#pragma once
#include <vector>

#include <ResourcesManager/MaterialsManager/core_materials_manager.h>

namespace poke {
namespace editor {
class EditorMaterialsManager : public CoreMaterialsManager {
public:
    //------------------------------ OVERRIDE ---------------------------------
    EditorMaterialsManager();
    ~EditorMaterialsManager() = default;

    void AddMaterial(const std::string& materialName) override;

    void Clear() override;

    void SetFromJson(const json& materialsJson) override;

    json ToJson() override;
    //-------------------------------------------------------------------------

    //---------------------------- EDITOR SPECIFICS ---------------------------
    /**
     * \brief Get the material type of the given material name. If the material is not registered, it return MaterialType::NONE.
     * \param materialName 
     * \return 
     */
    graphics::MaterialType GetMaterialType(const std::string& materialName);

    /**
     * \brief Get the material type of the given material name. If the material is not registered, it return MaterialType::NONE.
     * \param resourceID
     * \return
     */
    graphics::MaterialType GetMaterialType(ResourceID resourceID);

    /**
     * \brief Get a material skybox using the name of the material.
     * \param materialName 
     * \return 
     */
    //TODO(@Nico) Return const ref
    MaterialSkybox& GetMaterialSkybox(const std::string& materialName);

    /**
     * \brief Get a material skybox using the name of the material.
     * \param resourceID
     * \return
     */
    //TODO(@Nico) Return const ref
    MaterialSkybox& GetMaterialSkybox(ResourceID resourceID);

    /**
     * \brief Get a material diffuse using the name of the material.
     * \param materialName
     * \return
     */
    //TODO(@Nico) Return const ref
    MaterialDiffuse& GetMaterialDiffuse(const std::string& materialName);
    /**
     * \brief Get a material diffuse using the name of the material.
     * \param resourceID
     * \return
     */
    //TODO(@Nico) Return const ref
    MaterialDiffuse& GetMaterialDiffuse(ResourceID resourceID);

    /**
     * \brief Get a material trail using the name of the material.
     * \param materialName
     * \return
     */
    //TODO(@Nico) Return const ref
    MaterialTrail& GetMaterialTrail(const std::string& materialName);
    /**
     * \brief Get a material trail using the name of the material.
     * \param resourceID
     * \return
     */
    //TODO(@Nico) Return const ref
    MaterialTrail& GetMaterialTrail(ResourceID resourceID);

	/**
     * \brief Get a material particle using the name of the material.
     * \param materialName
     * \return
     */
     //TODO(@Nico) Return const ref
	MaterialParticle& GetMaterialParticle(const std::string& materialName);
	/**
	 * \brief Get a material particle using the name of the material.
	 * \param resourceID
	 * \return
	 */
	 //TODO(@Nico) Return const ref
	MaterialParticle& GetMaterialParticle(ResourceID resourceID);

    /**
     * \brief Get all skybox materials names.
     * \return 
     */
    const std::vector<std::string>& GetMaterialSkyboxNames() const;
    /**
     * \brief  Get all diffuse materials names.
     * \return
     */
    const std::vector<std::string>& GetMaterialDiffuseNames() const;
	/**
	 * \brief  Get all trail materials names.
	 * \return
	 */
	const std::vector<std::string>& GetMaterialTrailNames() const;
	/**
     * \brief  Get all particle materials names.
     * \return
     */
	const std::vector<std::string>& GetMaterialParticlesNames() const;

    /**
     * \brief Get a material name from its hash.
     * \param resourceID 
     * \return 
     */
    const std::string& GetMaterialName(ResourceID resourceID);

    /**
     * \brief Set a skybox material. The material must already exist.
     * \param material 
     * \param materialName 
     */
    void SetMaterial(
        const MaterialSkybox& material,
        const std::string& materialName);

    /**
     * \brief Set a skybox material. The material must already exist.
     * \param material 
     * \param resourceID 
     */
    void SetMaterial(const MaterialSkybox& material, ResourceID resourceID);

    /**
     * \brief Set a diffuse material. The material must already exist.
     * \param material
     * \param materialName
     */
    void SetMaterial(
        const MaterialDiffuse& material,
        const std::string& materialName);

    /**
     * \brief Set a diffuse material. The material must already exist.
     * \param material
     * \param resourceID
     */
    void SetMaterial(
        const MaterialDiffuse& material,
        ResourceID resourceID);

    /**
     * \brief Set a trail material.
     * \details The material must already exist.
     * \param material
     * \param materialName
     */
    void SetMaterial(
        const MaterialTrail& material,
        const std::string& materialName);

    /**
     * \brief Set a trail material.
     * \details The material must already exist.
     * \param material
     * \param resourceID
     */
    void SetMaterial(
        const MaterialTrail& material,
        ResourceID resourceID);

	/**
	 * \brief Set a particle material.
	 * \details The material must already exist.
	 * \param material
	 * \param materialName
	 */
	void SetMaterial(
		const MaterialParticle& material,
		const std::string& materialName);

	/**
	 * \brief Set a particle material.
	 * \details The material must already exist.
	 * \param material
	 * \param resourceID
	 */
	void SetMaterial(
		const MaterialParticle& material,
		ResourceID resourceID);
    //-------------------------------------------------------------------------
    void SetBaseMaterialNames();

private:

    std::vector<std::string> skyboxMaterialNames_;

    std::vector<std::string> diffuseMaterialNames_;

    std::vector<std::string> trailMaterialNames_;

    std::vector<std::string> particleMaterialNames_;
};
} //namespace editor
} //namespace poke
