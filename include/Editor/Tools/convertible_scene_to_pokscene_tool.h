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
// Author : Oleg Loshkin
// Co-Author :
// Date : 29.01.20
//----------------------------------------------------------------------------------
#pragma once

#include <Editor/Tools/tool.h>
#include <Utility/json_utility.h>
#include <Ecs/ecs_utility.h>

namespace poke {
namespace editor {
class Editor;

struct ConvertibleScene {
	ConvertibleScene(const char* fileName) : fileName(fileName) {};

	const char* fileName;
	json gameObjectsCount = json();
	json gameObjects = json();
	json resources = json();
	json chunksCount = json();
	json chunks = json();
	json chunkTimeEventsCount = json();
	json chunkTimeEvents = json();
	json chunkTriggerEventsCount = json();
	json chunkTriggerEvents = json();
};

struct ConversionLists {
	std::vector<std::string> materialNames = std::vector<std::string>();
	std::vector<std::string> meshNames = std::vector<std::string>();
	std::vector<std::string> audioNames = std::vector<std::string>();
    //TODO(@Duncan) If you're not using it the remove it.
	//std::vector<std::string> texturesNames = std::vector<std::string>();

    std::map<unsigned int, ecs::EntityIndex> unityIdToPokSceneId = std::map<unsigned int, ecs::EntityIndex>();
};

class ConvertibleSceneToPokSceneTool : public Tool {
public:
	// Constructor.
	explicit ConvertibleSceneToPokSceneTool(Editor& editor, bool defaultActive = false);

	// Tool inherited.
	void OnDrawImgui() override;
	void OnSetActive() override; 
	void OnSetInactive() override; 
private:
	inline static const unsigned int kFileNameLength = 64;
	char inputFileName_[kFileNameLength] = "";
	char outputFileName_[kFileNameLength] = "";

	bool Convert(const char* inputFileName, const char* outputFileName) const;
	bool WriteOutput(const ConvertibleScene& convertibleScene) const;

	// Game object component retrieving methods.
	json RetrieveTransformFrom(const json& gameObject) const;
	json RetrieveColliderFrom(const json& gameObject) const;
	json RetrieveSplineFrom(const json& gameObject) const;
	json RetrieveLightFrom(const json& gameObject) const;

    // Game object component setting methods.
	//TODO(@Duncan) This function doesn't need anything from the instance so you can make it static.
	json SetComponentFor(
		const ecs::ComponentType::ComponentType componentType, 
		const json& gameObject, 
		const json& component) const;
};
} //namespace editor
} //namespace poke
