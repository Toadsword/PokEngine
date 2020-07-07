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
// Author : Duncan Bourquard
// Date : 20.01.2020
//----------------------------------------------------------------------------------

#pragma once

#include <string>
#include <functional>
#include <vector>

#include <json.hpp>
using json = nlohmann::json;

namespace poke
{

/**
 * \brief Type of file.
 */
enum class FileType : char {
	DATA = 0,
	LOGS,
	JSON,
	SCENE,
	PREFAB,
	LANG,
	BINDING,
	LANG_CSV,
	FONTS,
	SHADER,
	VERT,
	FRAG,
	COMP,
	TEXTURE,
	IMAGE_CUBE,
    SKYBOX,
	COMPILED_SHADER,
    MESH,
	USER_PREFS,
	ENGINE_SETTING,
	APP_SETTING,
	POKCONVERTIBLESCENE,
    MATERIAL,
    TAGS,
	PARTICLE,
    SOUNDS,
    FMOD_BANK,
	CUSTOM
};

enum class FolderType : char {
	SAVE = 0, // For all the save data files
	ROM, // For the Read-Only files of the game
    SAVE_IN_ROM, // Used by tools, to save data inside ROM for future uses. Prevent crash on Switch.
    RESOURCE // For all the resources files that shouldn't be in the build 
};

class PokFileSystem {
public:
    /**
     * \brief Check if a file exists in the current file system.
     * \param filePath The full path to the file.
     * \return True if exists, false otherwise
     */
	static bool CheckFileExists(const std::string& filePath);

    /**
     * \brief Check if the current directory exists
     * \param dirName Name of the directory to check 
     * \return true if exists, false otherwise
     */
	static bool IsDirectory(const std::string& dirName);

    /**
     * \brief Iterate through the folder and subfolders and execute a function.
     * \param dirName Name of the directory
     * \param func Function to execute
     */
	static void IterateDirectory(
		const std::string& dirName, 
		std::function<void(std::string)> func
	);

    /**
     * \brief Iterate through the folder and subfolders and returns all the files
     * that can be found in the folder.
     * \param dirName Name of the directory
     * \param searchSubFolders Should we search in subfolders aswell?
     * \param returnFullPathName Should we return the full path or just the file name?
	 * \param alreadyInit (SWITCH ONLY) Mount the file system before iterating
     */
	static std::vector<std::string> GetAllFilesInDirectories(
	    const std::string& dirName, 
	    bool searchSubFolders = false, 
	    bool returnFullPathName = true, 
	    bool alreadyInit = false
    );
    /**
     * \brief Iterate through the folder and subfolders and returns all the folders found
     * that can be found in the folder.
     * \param dirName Name of the directory
     */
	static std::vector<std::string> GetAllFoldersInDirectories(
		const std::string& dirName
    );

	/**
     * \brief Iterate through the folder and subfolders and returns all the files
     * that can be found in the folder.
     * \param fileType Type of file we search
     * \param folderType Folder to search on
     * \param searchSubFolders Should we search in subfolders aswell?
     * \param returnFullPathName Should we return the full path or just the file name?
     * \param alreadyInit (SWITCH ONLY) Mount the file system before iterating
     */
	static std::vector<std::string> GetAllFilesInDirectories(
		FileType fileType,
		FolderType folderType,
		bool searchSubFolders = false,
		bool returnFullPathName = true,
		bool alreadyInit = false
	);

    /**
     * \brief Create a directory
     * \param dirName Name of directory
     * \return True if created, false otherwise
     */
	static bool CreateDirectory(const std::string& dirName);

    /**
     * \brief Delete a directory. Must be empty unless removeAll is set to true
     * \param dirName Name of directory
     * \param removeAll True to remove everything in directory, false to remove an empty directory
     * \return True if directory is deleted, false otherwise
     */
	static bool RemoveDirectory(const std::string& dirName, bool removeAll = true);

    /** 
     * \brief Open, or create if not existing, a file and write the content in it
     * \param fileName Name of the file
     * \param fileContent Content to put in the file
     * \param fileType Type of file. If specified, premake the path and the extension of the file.
     * \return True if written, false otherwise
     */
	static bool WriteFile(
	    const std::string& fileName, 
	    const std::string& fileContent, 
	    const FileType fileType = FileType::CUSTOM, 
		const FolderType folderType = FolderType::SAVE
    );

    /** 
     * \brief Open, or create if not existing, a file and write the content in it
     * \param fileName Name of the file
     * \param fileContent Content to put in the file in json format
     * \param fileType Type of file. If specified, premake the path and the extension of the file.
     * \return True if written, false otherwise
     */
	static bool WriteFile(
	    const std::string& fileName, 
	    const json& fileContent,
	    FileType fileType = FileType::CUSTOM,
		FolderType folderType = FolderType::SAVE
    );

    /**
     * \brief Read a file and return its content
     * \param fileName Name of the file
     * \param fileType Type of file. If specified, premake the path and the extension to the file.
     * \param folderType Type of folder.
     * \return The content of the file if able to read, empty string otherwise
     */
	static std::string ReadFile(
	    const std::string& fileName, 
	    const FileType fileType = FileType::CUSTOM, 
	    const FolderType folderType = FolderType::ROM
    );

    /**
    * \brief Read a json file and return its content
    * \param fileName Name of the file
    * \param fileType Type of file. If specified, premake the path and the extension to the file.
    * \param folderType Type of folder.
    * \return The content of the file if able to read, empty json otherwise
    */
	static json ReadJsonFile(
	    const std::string& fileName, 
        const FileType fileType = FileType::CUSTOM, 
	    const FolderType folderType = FolderType::ROM
    );
	static json ReadJsonFile(
		const char* fileName,
		const FileType fileType = FileType::CUSTOM,
		const FolderType folderType = FolderType::ROM
	);

    /**
     * \brief Delete a file
     * \param fileName Name of the file
     * \param fileType Type of file. If specified, premake the path and the extension of the file.
     * \param folderType Type of folder.
     * \return True if deleted, false otherwise
     */
	static bool DeleteFile(
	    const std::string& fileName, 
	    const FileType fileType = FileType::CUSTOM,
	    const FolderType folderType = FolderType::ROM
    );

    /**
     * \brief Return the path of the type of file.
     * \param fileType Type of file. If specified, premake the path and the extension of the file.
     * \param folderType Type of folder.
     * \return String of the path of the type of file specified
     */
	static std::string GetPath(const FileType fileType, const FolderType folderType);

	/**
     * \brief Return the absolute full path of the type of file.
     * \param fileType Type of file. If specified, premake the path and the extension of the file.
     * \param folderType Type of folder.
     * \return String of the path of the type of file specified
     */
	static json GetAbsoluteFullPath(
		const std::string& fileName,
		const FileType fileType = FileType::CUSTOM,
		const FolderType folderType = FolderType::ROM
	);
	static std::string GetAbsoluteFullPath(
		const char* filename,
		const FileType fileType = FileType::CUSTOM,
		const FolderType folderType = FolderType::ROM);

	/**
     * \brief Get the subpath from the folder.
     *        Subpath : "subfolder/filename.extension"
     *        Instead of Fullpath : "../Appdata/resources/filetype/subfolder/filename.extension
     * \param fullPath fullpath to the desiredPath
     * \param fileType Type of file.
     * \param folderType Type of folder.
     * \param withExtension Specify if we remove the extension or note.
     * \return String of the path of the type of file specified
     */
	static std::string GetSubPathFromFullPath(
		const std::string& fullPath, 
		const FileType fileType, 
		const FolderType folderType,
		bool withExtension = true);

    /**
     * \brief Return the extension of the type of file
     * \param fileType Type of file. If specified, premake the path and the extension of the file.
     * \return String of the extension of the file
     */
	static std::string GetExtension(const FileType fileType);

    /**
     * \brief Return the full path of the file.
     * \param fileName Name of the file
     * \param fileType Type of file. If specified, premake the path and the extension of the file.
     * \param folderType Type of folder.
     * \return Full path to the file.
     */
	static std::string GetFullPath(
	    const std::string& fileName, 
	    const FileType fileType = FileType::CUSTOM, 
	    const FolderType folderType = FolderType::ROM
    );

	/**
     * \brief Return the full path of the file.
     * \param fileName Name of the file
     * \param fileType Type of file. If specified, premake the path and the extension of the file.
     * \param folderType Type of folder.
     * \return Full path to the file.
     */
	static std::string GetFullPath(
		const char* fileName,
		const FileType fileType = FileType::CUSTOM,
		const FolderType folderType = FolderType::ROM
	);

    /**
	 * \brief Setup the base path for all the file. Need to be done only at startup.
	 *          It's an option used for "test" solutions.
	 * \param newBasePath 
	 */
	static void SetupBasePath(const std::string& newBasePath);

	/**
    * \brief Return the string equivalent of the ResourceType enum value
    * \param resourceType the ResourceType wanted
    * \return string name of the resource type
    **/
	static std::string ResourceTypeToString(FileType resourceType);

	/**
	* \brief Return the FileType of a given filePath
	* \param filePath the path of the file
	* \return FileType the type of the file
	**/
	static FileType GetFileType(std::string& filePath);


private:
	PokFileSystem() = default;

	inline static std::string basePath_ = "";
};
} // namespace poke