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
// Author : Guillaume Volygesi
// Co-Author :
// Date : 10.12.19
//----------------------------------------------------------------------------------
#pragma once

#include <map>

#include <Editor/Tools/tool.h> 
#include <Utility/file_system.h>
#include <Inputs/input_manager.h>

namespace poke { 
namespace editor {
class Editor;

/**
* \brief Input Binding tool
*/
class InputBindingTool : public Tool
{
public:
	explicit InputBindingTool(Editor& editor, bool defaultActive = false);
	
	void OnDrawImgui() override;

	void OnSetActive() override;

	void OnSetInactive() override;   

private:	
	input::Action action_ = input::Action::UP;
			
	std::vector<std::string> actionsArray_{};
	std::vector<std::string> switchInputsArray_{};
	std::vector<std::string> controllerInputsArray_{};
	std::vector<std::string> pcInputsArray_{};
	std::vector < std::map<std::string, int>> bindingFromJson_;

	inline static const char* actionCurrent_ = nullptr;
	inline static const char* switchInputCurrent_ = nullptr;
	inline static const char* controllerInputCurrent_ = nullptr;
	inline static const char* pcInputCurrent_ = nullptr;

	int indexPc_; 
	int indexSwitch_; 
	int indexController_; 
	int indexAction_; 
	/**
	* \brief bind input to action and save to json
	*/
	void Bind(); 
	/**
	* \brief read bindingFromJson and set current inputs for the current action
	*/
	void SetBindingsInput();
	/**
	* \brief reserve capacity for all std::vector
	*/
	void SetArraysOfInputs();
	/**
	* \brief set jsonInput struct from json
	*/
	void LoadBindingsFromJson(); 

	//TODO(@Gui) data can be const &
	void CreateToJson(json data) const; 

	/**
	* \brief translate actionCurrent string to enum
	*/
	void SetActionEnum(const std::string &string);
};
} //namespace editor
} //namespace poke