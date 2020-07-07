#include <Editor/Tools/resource_accessor_tool.h>

#include <string>
#include <tuple>
#include <filesystem>

#include <imgui.h>

#include <Utility/file_system.h>

namespace poke {
namespace editor {

std::vector<Resource> ResourceAccessorTool::GetResourceOfType(
	FileType resourceType, 
	FolderType folderType)
{
    std::vector<Resource> returnResources;
    if(resourceType == FileType::IMAGE_CUBE || resourceType == FileType::SKYBOX) {
		std::vector<std::string> folderPaths = PokFileSystem::GetAllFoldersInDirectories(
			PokFileSystem::GetPath(resourceType, folderType)
		);
		for (std::string& folderPath : folderPaths) {
			Resource resource;
			const size_t folderNamePos = std::min(
				folderPath.find_last_of('/'),
				folderPath.find_last_of('\\')
			);
			resource.path = folderPath;
			resource.name = folderPath.substr(
				folderNamePos + 1,
				folderPath.size() - folderNamePos
			);
			resource.extension = "";
			resource.type = resourceType;
			resource.typeString = PokFileSystem::ResourceTypeToString(resource.type);
			returnResources.push_back(resource);
		}
    }else {
		std::vector<std::string> filePaths = PokFileSystem::GetAllFilesInDirectories(
		    PokFileSystem::GetPath(resourceType, folderType),
		    true
	    );
		for (std::string filePath : filePaths) {
			Resource resource;
			const size_t fileNamePos = std::min(
				filePath.find_last_of('/'),
				filePath.find_last_of('\\')
			);
			const int fileExtPos = filePath.find_last_of('.');
			resource.path = filePath;
			resource.name = filePath.substr(
				fileNamePos + 1,
				filePath.size() - fileNamePos - (filePath.size() - fileExtPos) - 1
			);
			resource.extension = filePath.substr(
				fileExtPos,
				filePath.size() - fileExtPos
			);
			resource.type = PokFileSystem::GetFileType(filePath);
			resource.typeString = PokFileSystem::ResourceTypeToString(resource.type);
			returnResources.push_back(resource);
		}
    }
	
    if(resourceType != FileType::CUSTOM) {
		returnResources = FindResourcesByType(returnResources, resourceType);
    }

	return returnResources;
}


std::tuple<Resource, bool> ResourceAccessorTool::DataDisplay(
	std::vector<Resource>& resources, 
	FileType resourceType, 
	FolderType folderType)
{
    Resource resourceToReturn = INVALID_RESOURCE;
	bool isSelected = false;

	if (ImGui::BeginPopupModal("ResourceAccessor", &isModalOpened_)) {
		if (ImGui::Button("refresh")) {
			currentResources_ = GetResourceOfType(resourceType, folderType);
		} 
		ImGui::Columns(3);
		ImGui::Text("Name");
		size_t index = 0;
		for (const Resource& file : resources) {
			if (ImGui::Selectable(
				file.name.c_str(), 
				currentlySelectedIndex_ == index, 
				ImGuiSelectableFlags_DontClosePopups)) 
			{
                if(currentlySelectedIndex_ == index) {
					resourceToReturn = file;
					isSelected = true;
                }
			    currentlySelectedIndex_ = index;
			}
			index++;
		}

		ImGui::NextColumn();
		ImGui::Text("Type");
		for (const Resource& file : resources) {
			ImGui::Text(file.typeString.c_str());
		}

		ImGui::NextColumn();
		ImGui::Separator();
		ImGui::Text("Path");
		for (const Resource& file : resources) {
			ImGui::Text(file.path.c_str());
		}
		ImGui::Columns(1);

		ImGui::Separator();
        if(currentlySelectedIndex_ != 0 && 
			IsResourceValid(resources.at(currentlySelectedIndex_))
			) {
            if(ImGui::Button("Validate")) {
			    resourceToReturn = resources.at(currentlySelectedIndex_);
			    isSelected = true;
            }
        }

		ImGui::EndPopup();
	}

	return std::make_tuple(resourceToReturn, isSelected);
}

std::vector<Resource> ResourceAccessorTool::FindResourcesByName(std::vector<Resource>& resources, std::string& name) {
	std::vector<Resource> resourcesToReturn;
	for(Resource& resource : resources) {
		if(resource.name == name) {
			resourcesToReturn.push_back(resource);
		}
	}
	return resourcesToReturn;
}

std::vector<Resource> ResourceAccessorTool::FindResourcesByType(std::vector<Resource>& resources, FileType type) {
	std::vector<Resource> resourcesToReturn;
	for (Resource& resource : resources) {
		if (resource.type == type) {
			resourcesToReturn.push_back(resource);
		}
	}
	return resourcesToReturn;
}

std::vector<Resource> ResourceAccessorTool::FindResourcesByExtension(std::vector<Resource>& resources, std::string& extension) {
	std::vector<Resource> resourcesToReturn;
	for (Resource& resource : resources) {
		if (resource.extension == extension) {
			resourcesToReturn.push_back(resource);
		}
	}
	return resourcesToReturn;
}

Resource ResourceAccessorTool::DisplayGetResource(
	const char* buttonText,
    DisplayType displayType,
	FileType resourceType, 
	FolderType folderType)
{
	bool isDataSelected = false;
	Resource selectedResource = INVALID_RESOURCE;
	callIndex_++;

	bool displayed = false;
    switch(displayType) {
	    case DisplayType::BUTTON: {   
            displayed = ImGui::Button(buttonText);
            break;
	    }
		case DisplayType::MENU_ITEM: {	        
			displayed = ImGui::MenuItem(buttonText);
			break;
	    }
		case DisplayType::DISPLAYED: {
			displayed = true;
			break;
	    }
    }

    //Modal opened
    if(displayed && !isModalOpened_) {
		isModalOpened_ = true;

		currentResources_.clear();
		currentlySelectedIndex_ = 0;
		if (currentResources_.empty()) {
			currentResources_ = GetResourceOfType(resourceType, folderType);
		}
		indexFlagOpenened_ = callIndex_;
		ImGui::OpenPopup("ResourceAccessor");
    }

    //Modal manager
	if(isModalOpened_ && indexFlagOpenened_ == callIndex_) {
	    std::tie(selectedResource, isDataSelected) = 
			DataDisplay(currentResources_, resourceType, folderType);
		if (isDataSelected) {
			isModalOpened_ = false;
			indexFlagOpenened_ = 0;
			return selectedResource;
		}
    }

	return selectedResource;
}

bool ResourceAccessorTool::IsResourceValid(const Resource& resource) {
	return !(resource.name == "invalid" &&
		resource.extension == "invalid" &&
		resource.path == "invalid" &&
		resource.typeString == "invalid" &&
		resource.type == FileType::CUSTOM);
}

} //namespace editor
} //namespace poke