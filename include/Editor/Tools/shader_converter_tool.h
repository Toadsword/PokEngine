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
// Co-Author :
// Date : 03.12.2019
//----------------------------------------------------------------------------------
#pragma once

#include <Editor/Tools/tool.h>
#include <Utility/file_system.h>
#include <Editor/Tools/resource_accessor_tool.h>

namespace poke::editor {
class Editor;

#define INVALID_INDEX -1

/**
 * \brief Sample tool that work as a console
 */
class ShaderConverterTool final : public Tool {
public:
	explicit ShaderConverterTool(Editor& editor, bool defaultActive = false);

	void OnDrawImgui() override;

	/**
     * \brief Is called when the tool is opened
     */
	void OnSetActive() override;

	/**
	 * \brief Is called when the tool is closed
	 */
	void OnSetInactive() override;

private:
	void ConvertToShader(std::string& fileName);
	
	bool ReadLine(std::string& fileContent, std::string& result);
	std::string Trim(std::string& str);
	bool CheckEndOfLine(std::string& line);
	std::string GetInfo(
		std::string& line, 
		const char* searchStr, 
		std::string& fileContent, 
		bool forceNewLine = true
	);
	void AnalyseFile(std::string& fileContent, json& result);

	int GetStageFlags(const std::string& value);
	int GetSizeOfType(const std::string& value);
	int GetGlType(const std::string& value);

	Resource selectedShader_ = INVALID_RESOURCE;
	const int kDefaultSet = 63;
};
} // namespace poke::editor
