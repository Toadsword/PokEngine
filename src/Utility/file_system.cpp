#include <Utility/file_system.h>

#include <fstream>
#include <vector>

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include <CoreEngine/cassert.h> 

namespace poke {

bool PokFileSystem::CheckFileExists(const std::string& filePath) 
{
	const fs::path kPath = filePath;
	return fs::exists(kPath);
}

bool PokFileSystem::IsDirectory(const std::string& dirName) 
{
	if (dirName.empty()) return true;

	const fs::path kPath = dirName;
	return fs::is_directory(kPath);
}

void PokFileSystem::IterateDirectory(
	const std::string& dirName, 
	std::function<void(std::string)> func)
{
	if (IsDirectory(dirName)) {
		for (auto& kPath : fs::directory_iterator(dirName)) {
			func(kPath.path().generic_string());
		}
	}
}

std::vector<std::string> PokFileSystem::GetAllFilesInDirectories(
	const std::string& dirName, 
	bool searchSubFolders, 
	bool returnFullPathName,
    bool alreadyInit) 
{
	std::vector<std::string> list = std::vector<std::string>();

	for (const auto& kEntry : fs::directory_iterator(dirName)) {
		std::string subPath = dirName + kEntry.path().filename().generic_string();
		if (IsDirectory(subPath + "/"))	{
			if (searchSubFolders) {
				std::vector<std::string> subList = 
					GetAllFilesInDirectories(
						subPath + "/", 
						searchSubFolders, 
						returnFullPathName, 
						true
					);
				list.insert(
					list.end(), 
					subList.begin(), 
					subList.end()
				);
			}
		} else {	
			if(returnFullPathName) {
				list.push_back(subPath);
			} else {   
				list.push_back(kEntry.path().filename().generic_string());
			}
		}
	}

	return list;
}

std::vector<std::string> PokFileSystem::GetAllFoldersInDirectories(
	const std::string& dirName)
{
	std::vector<std::string> list = std::vector<std::string>();

	for (const auto& entry : fs::directory_iterator(dirName)) {
		std::string subPath = dirName + entry.path().filename().generic_string();
		if (IsDirectory(subPath + "/")) {
			list.push_back(subPath);
		}
	}

	return list;
}

bool PokFileSystem::CreateDirectory(const std::string& dirName) 
{
   if (!IsDirectory(dirName)) {
		return fs::create_directories(dirName);
	} else {
		return false;
	}
}

bool PokFileSystem::RemoveDirectory (
	const std::string& dirName, 
	const bool removeAll) 
{
	if (removeAll) {
		return fs::remove_all(dirName);
	} else {
		return fs::remove(dirName);
	}
}

std::vector<std::string> PokFileSystem::GetAllFilesInDirectories(
	FileType fileType,
	FolderType folderType,
	bool searchSubFolders,
	bool returnFullPathName,
	bool alreadyInit)
{
	return PokFileSystem::GetAllFilesInDirectories(
		GetPath(fileType, folderType),
		searchSubFolders,
		returnFullPathName,
		alreadyInit);
}

bool PokFileSystem::WriteFile(
	const std::string& fileName, 
	const std::string& fileContent,
	const FileType fileType,
    const FolderType folderType)
{
	CreateDirectory(GetPath(fileType, folderType));

	std::ofstream outputFile;
	outputFile.open(GetFullPath(fileName, fileType, folderType));
	outputFile << fileContent;
	outputFile.close();

	return true;
}

bool PokFileSystem::WriteFile(
	const std::string& fileName, 
	const json& fileContent, 
	const FileType fileType,
	const FolderType folderType)
{
	CreateDirectory(GetPath(fileType, folderType));

	std::ofstream outputFile;
	outputFile.open(GetFullPath(fileName, fileType, folderType));
	outputFile << std::setw(4) << fileContent << std::endl;

	outputFile.close();

	return true;
}

std::string PokFileSystem::ReadFile(
	const std::string& fileName, 
	const FileType fileType, 
	const FolderType folderType)
{
	const std::string kPath = GetFullPath(fileName, fileType, folderType);
	if (!CheckFileExists(kPath)) {
		return std::string();
	}

	std::string strResult = std::string();
	std::ifstream file(kPath, std::ios::ate | std::ios::binary);

	if (file.is_open()) {
		const auto kFileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(kFileSize);

		file.seekg(0);
		file.read(buffer.data(), kFileSize);
		strResult = std::string(buffer.begin(), buffer.end());
	}

	file.close();

	return strResult;
}

json PokFileSystem::ReadJsonFile(
	const std::string& fileName, 
	const FileType fileType, 
	const FolderType folderType)
{
	return ReadJsonFile(fileName.c_str(), fileType, folderType);
}

json PokFileSystem::ReadJsonFile(const char * fileName, const FileType fileType, const FolderType folderType)
{
	const std::string kPath = GetFullPath(fileName, fileType, folderType);

	std::cout << kPath << "\n";

	json jsonContent = json();
	if (CheckFileExists(kPath)) {
		std::ifstream jsonFile(kPath);
		cassert(
			jsonFile.peek() != std::ifstream::traits_type::eof(),
			"[JSON ERROR] EMPTY JSON FILE at: " + kPath
		);

		jsonFile >> jsonContent;
		jsonFile.close();
	}else {
		std::cout << "File not found\n";
	}

	return jsonContent;
}

bool PokFileSystem::DeleteFile(
	const std::string& fileName, 
	const FileType fileType, 
	const FolderType folderType) 
{
	const std::string kPath = GetFullPath(fileName, fileType, folderType);

	if (!CheckFileExists(kPath)) {
		return false;
	}

	const bool kDeleteResult = remove(kPath.c_str());

	return kDeleteResult;
}

std::string PokFileSystem::GetPath(
	const FileType fileType, 
	const FolderType folderType)
{
	std::string path = basePath_;
	switch (folderType) {
	    case FolderType::ROM: 
	    case FolderType::SAVE_IN_ROM: path += "../AppData/"; break;
	    case FolderType::SAVE: path += "../save/"; break;
		case FolderType::RESOURCE: path += "../resources/"; break;
	}

	switch (fileType) {
	    case FileType::DATA: return path;
	    case FileType::LOGS: return path + "logs/";
	    case FileType::JSON: return path;
	    case FileType::SCENE: return path + "resources/scenes/";
	    case FileType::PREFAB: return path + "resources/prefabs/";
	    case FileType::LANG: return path + "translation/";
		case FileType::LANG_CSV: return path + "translation/";
	    case FileType::FONTS: return path + "fonts/";
	    case FileType::SHADER:
	    case FileType::VERT:
	    case FileType::FRAG:		
	    case FileType::COMP: return path + "shaders/";
	    case FileType::TEXTURE: return path + "resources/textures/";
	    case FileType::IMAGE_CUBE: 
	    case FileType::SKYBOX: return path + "resources/skybox/";
		case FileType::COMPILED_SHADER: return path + "shaders/compiled/";
		case FileType::MESH: return path + "resources/meshes/";
		case FileType::USER_PREFS: return path + "userPrefs/";
		case FileType::ENGINE_SETTING:
		case FileType::APP_SETTING: return path + "setting/";
		case FileType::POKCONVERTIBLESCENE: return path;
		case FileType::MATERIAL: return path + "resources/materials/";
		case FileType::BINDING: return path + "bindings/";
		case FileType::PARTICLE: return path + "resources/particles/";
		case FileType::SOUNDS: return path + "resources/sounds/";
		case FileType::FMOD_BANK: return path + "resources/fmodBanks/";
	    default: return path;

	}
}

json PokFileSystem::GetAbsoluteFullPath(const std::string& fileName,
    const FileType fileType,
    const FolderType folderType)
{
	return GetAbsoluteFullPath(fileName.c_str(), fileType, folderType);
}

std::string PokFileSystem::GetAbsoluteFullPath(
	const char* fileName,
	const FileType fileType,
	const FolderType folderType)
{
	return fs::absolute(GetFullPath(fileName, fileType, folderType)).generic_string();
}

std::string PokFileSystem::GetSubPathFromFullPath(const std::string& fullPath,
                                                  const FileType fileType,
                                                  const FolderType folderType,
                                                  bool withExtension)
{
	std::string folderPath = PokFileSystem::GetPath(fileType, folderType);
	size_t nameSize = fullPath.size() - folderPath.size();

    if(!withExtension) {
		nameSize -= GetExtension(fileType).size();
    }

	return fullPath.substr( folderPath.size(), nameSize);
}

std::string PokFileSystem::GetExtension(const FileType fileType) 
{
	switch (fileType) {
	    case FileType::LOGS: return std::string(".poklog");
	    case FileType::JSON: return std::string(".json");
	    case FileType::SCENE: return std::string(".pokscene");
	    case FileType::LANG: return std::string(".poklang");
		case FileType::LANG_CSV: return std::string(".csv");
	    case FileType::FONTS: return std::string(".ttf");
	    case FileType::SHADER: return std::string(".shader");
	    case FileType::PREFAB: return std::string(".pokprefab");
	    case FileType::VERT:
	    case FileType::FRAG:
	    case FileType::COMP: return std::string(".spv");
		case FileType::COMPILED_SHADER: return std::string(".pokpreshader");
		case FileType::USER_PREFS: return std::string(".userprefs");
		case FileType::ENGINE_SETTING: return std::string(".pokenginesetting");
		case FileType::APP_SETTING: return std::string(".pokappsetting");
		case FileType::POKCONVERTIBLESCENE: return std::string(".pokconvertiblescene");
		case FileType::MATERIAL: return std::string(".pokmaterial");
		case FileType::BINDING: return std::string(".pokinput");
		case FileType::PARTICLE: return std::string(".pokparticle");
		case FileType::TAGS: return std::string(".poktag");
		case FileType::FMOD_BANK: return std::string(".bank");
	    default: return std::string();
	}
}

std::string PokFileSystem::GetFullPath(
	const std::string& fileName,
	const FileType fileType, 
	const FolderType folderType)
{
	return GetFullPath(fileName.c_str(), fileType, folderType);
}

std::string PokFileSystem::GetFullPath(const char * fileName, const FileType fileType, const FolderType folderType)
{
	std::string result = std::string();
	result += GetPath(fileType, folderType) + fileName + GetExtension(fileType);
	return result;
}

void PokFileSystem::SetupBasePath(const std::string& newBasePath) 
{
	basePath_ = newBasePath;
}

std::string PokFileSystem::ResourceTypeToString(FileType resourceType) {
	switch (resourceType) {
	case FileType::FONTS: return "FONTS";
	case FileType::COMP: return "COMP";
	case FileType::LOGS: return "LOGS";
	case FileType::MESH: return "MESH";
	case FileType::MATERIAL: return "MATERIAL";
	case FileType::TEXTURE: return "TEXTURE";
	case FileType::SKYBOX: return "SKYBOX";
	case FileType::IMAGE_CUBE: return "IMAGE_CUBE";
	case FileType::CUSTOM: return "CUSTOM";
	case FileType::DATA: return "DATA";
	case FileType::JSON: return "JSON";
	case FileType::SCENE: return "SCENE";
	case FileType::PREFAB: return "PREFAB";
	case FileType::LANG: return "LANG";
	case FileType::SHADER: return "SHADER";
	case FileType::FRAG: return "FRAG";
	case FileType::COMPILED_SHADER: return "COMPILED_SHADER";
	case FileType::USER_PREFS: return "USER_PREFS";
	case FileType::ENGINE_SETTING: return "ENGINE_SETTING";
	case FileType::APP_SETTING: return "APP_SETTING";
	case FileType::POKCONVERTIBLESCENE: return "POKCONVERTIBLESCENE";
	case FileType::VERT: return "VERT";
	case FileType::TAGS: return "TAGS";
	case FileType::FMOD_BANK: return "FMOD_BANK";
	default: return "UNKNOWN";
	}
}

FileType PokFileSystem::GetFileType(std::string& filePath) {
	int indexLastDot = filePath.find_last_of(".");
	std::string fileExtention = filePath.substr(
		indexLastDot, 
		filePath.size() - indexLastDot
	);

	if (fileExtention == ".poklog") { return FileType::LOGS; }
	if (fileExtention == ".json") { return FileType::JSON; }
	if (fileExtention == ".pokscene") { return FileType::SCENE; }
	if (fileExtention == ".poklang") { return FileType::LANG; }
	if (fileExtention == ".csv") { return FileType::LANG_CSV; }
	if (fileExtention == ".ttf") { return FileType::FONTS; }
	if (fileExtention == ".shader") { return FileType::SHADER; }
	if (fileExtention == ".pokprefab") { return FileType::PREFAB; }
	if (fileExtention == ".pokvert") { return FileType::VERT; }
	if (fileExtention == ".pokfrag") { return FileType::FRAG; }
	if (fileExtention == ".spv") { return FileType::COMP; }
	if (fileExtention == ".pokpreshader") { return FileType::COMPILED_SHADER; }
	if (fileExtention == ".userprefs") { return FileType::USER_PREFS; }
	if (fileExtention == ".pokenginesetting") { return FileType::ENGINE_SETTING; }
	if (fileExtention == ".pokappsetting") { return FileType::APP_SETTING; }
	if (fileExtention == ".pokconvertiblescene") { return FileType::POKCONVERTIBLESCENE; }
	if (fileExtention == ".pokmaterial") { return FileType::MATERIAL; }
	if (fileExtention == ".pokparticle") { return FileType::PARTICLE; }
	if (fileExtention == ".obj") { return FileType::MESH; }
	if (fileExtention == ".png" ||
		fileExtention == ".tga" ||
		fileExtention == ".jpg") {
		return FileType::TEXTURE;
	}
	if (fileExtention == ".poktag") { return FileType::TAGS;  }
	if (fileExtention == ".bank") { return FileType::FMOD_BANK;  }

	return FileType::CUSTOM;
}
} // namespace poke
