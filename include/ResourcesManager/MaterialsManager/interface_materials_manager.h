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
#include <string>

#include <Utility/json_utility.h>
#include <ResourcesManager/resource_type.h>

namespace poke {

namespace graphics {
class Material;
} //namespace graphics

class IMaterialsManager {
public:
    virtual ~IMaterialsManager() = default;

	/**
	 * \brief Add a material.
	 * \param materialName It must be the name of the file of the material. This function is slower than if using a json as a parameter.
	 */
    virtual void AddMaterial(const std::string& materialName) = 0;

	/**
	 * \brief Get a material.
	 * \param materialName It's the name of the file of the material. This function is slower than the version with the hash.
	 */
    virtual graphics::Material& GetMaterial(const std::string& materialName) = 0;
	/**
	 * \brief Get a material.
	 * \param resourceID It's the id of the name of the file.
	 */
    virtual graphics::Material& GetMaterial(ResourceID resourceID) = 0;

    /**
	 * \brief Get default material's id.
	 * \return 
	 */
	virtual ResourceID GetDefaultMaterialID() = 0;

	/**
	 * \brief Get default skybox material's id.
	 * \return
	 */
	virtual ResourceID GetDefaultSkyboxMaterialID() = 0;

    /**
     * \brief Delete all texture registered.
     */
    virtual void Clear() = 0;

    /**
     * \brief Resize the manager to the given size. If the size is smaller than the actual size and some texture are still in, they will be erased.
     * \param newSize must be greater than 0
     */
    virtual void Resize(size_t newSize) = 0;

	virtual void SetFromJson(const json& materialsJson) = 0;

	virtual json ToJson() = 0;
};
} //namespace poke
