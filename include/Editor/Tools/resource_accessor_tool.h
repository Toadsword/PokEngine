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
// Author : Stephen Grosjean
// Co-Author : Duncan Bourquard
// Date : 20.02.2020
//----------------------------------------------------------------------------------
#pragma once

#include <vector>
#include <string>
#include <Utility/file_system.h>

#define INVALID_RESOURCE Resource{"invalid", "invalid", "invalid", "invalid", FileType::CUSTOM};

namespace poke {
namespace editor {

struct Resource {
	std::string path;
	std::string name;
	std::string typeString;
	std::string extension;
	FileType type;
};

class ResourceAccessorTool {
public:

    enum class DisplayType : char{
        BUTTON = 0,
        MENU_ITEM,
        DISPLAYED
    };

	/**
	* \brief Return a list of Resource of a given ResourceType
	* \param resourceType the ResourceType wanted
	* \param folderType Folder where to search
	* \return vector of Resource of the wanted type
	**/
	static std::vector<Resource> GetResourceOfType(
		FileType resourceType = FileType::CUSTOM, 
		FolderType folderType = FolderType::ROM
	);

	/**
	* \brief Display all resources in the vector in a table with a selectable Resource
	* \param resources the vector of the resources
	* \param resourceType Type of resource to display by default
	* \param folderType Where to get the resource
	* \return std::tuple<Resource, bool> the selected resource and true if selection is confirmed
	**/
	static std::tuple<Resource, bool> DataDisplay(
		std::vector<Resource>& resources, 
		FileType resourceType = FileType::CUSTOM, 
		FolderType folderType = FolderType::ROM
	);

	/**
	* \brief Return all resources with that name
	* \param resources the Resource vector
	* \param name the name of the resource
	* \return std::vector<Resource> the vector of all resources with that name
	**/
	static std::vector<Resource> FindResourcesByName(
		std::vector<Resource>& resources, 
		std::string& name
	); 

	/**
	* \brief Return all resources with that type
	* \param resources the Resource vector
	* \param type the FileType of the resource
	* \return std::vector<Resource> the vector of all resources with that FileType
	**/
	static std::vector<Resource> FindResourcesByType(
		std::vector<Resource>& resources, 
		FileType type
	);

	/**
	* \brief Return all resources with that extension
	* \param resources the Resource vector
	* \param extension the extension of the resource
	* \return std::vector<Resource> the vector of all resources with that extension
	**/
	static std::vector<Resource> FindResourcesByExtension(
		std::vector<Resource>& resources, 
		std::string& extension
	);

    /**
	 * \brief Button to display that calls the necessary functions.
	 * \param buttonText Text to display on the button.
	 * \param resourceType Type of resource to display
	 * \param folderType Where to search for resources
	 * \return The resource once selected. Null if aborted.
	 */
	static Resource DisplayGetResource(const char* buttonText,
		DisplayType displayType = DisplayType::BUTTON,
		FileType resourceType = FileType::CUSTOM,
		FolderType folderType = FolderType::ROM);

    /**
	 * \brief Check if resource if valid.
	 * \param resource The resource to check.
	 * \return True if valid, false otherwise.
	 */
	static bool IsResourceValid(const Resource& resource);

	static void ResetIndexCount() { callIndex_ = 0; };
private:
	inline static std::vector<Resource> currentResources_ = std::vector<Resource>();
	inline static bool isModalOpened_ = false;
	inline static int callIndex_ = 0;
	inline static int indexFlagOpenened_ = 0;
	inline static size_t currentlySelectedIndex_ = 0;
};
} //namespace editor
} //namespace poke