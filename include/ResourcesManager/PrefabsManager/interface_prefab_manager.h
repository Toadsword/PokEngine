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
// Date : 14.03.2020
//----------------------------------------------------------------------------------
#pragma once
#include <Ecs/Prefabs/prefab.h>
#include <string_view.hpp>

namespace poke {
class IPrefabsManager {
public:
    virtual ~IPrefabsManager() = default;

    /**
	 * \brief Instantiate a prefab, if the name doesn't exist, it return an empty vector.
	 * \param prefabName 
	 * \return A vector of every entities created from this prefab.
	 */
	virtual std::vector<ecs::EntityIndex> Instantiate(const nonstd::string_view& prefabName) const = 0;

    /**
     * \brief Add a prefab to the manager from its name.
     * \param prefabFileName the name of the prefab file without extension.
     */
    virtual void AddPrefab(const std::string& prefabFileName) = 0;

    /**
     * \brief Get a prefab from its name
     * \param prefabName 
     * \return 
     */
    virtual const ecs::Prefab& GetPrefab(const nonstd::string_view& prefabName) const = 0;

    /**
	 * \brief Get all prefabs ids stored in the manager.
	 * \return 
	 */
	virtual const std::vector<ResourceID>& GetPrefabsIDs() const = 0;

	virtual void Clear() = 0;
	virtual void Resize(size_t newSize) = 0;

    virtual json ToJson() = 0;
    virtual void SetFromJson(const json& prefabsJson) = 0;
};
} //namespace poke
