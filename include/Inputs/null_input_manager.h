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
// Author : Luca Floreau
// Co-Author :
// Date : 25.02.20
//----------------------------------------------------------------------------------
#pragma once

#include <string>

#include <Inputs/interface_input_manager.h>

namespace poke::input {
class NullInputManager final : public IInputManager
{
public:
	NullInputManager() = default;
	~NullInputManager() = default;

	void OnAppBuild() override {}

	void BindFromJson() override {}

	void OnPreUserInputs() override {}

	bool IsKeyDown(KeyCode key) const override
	{
		key;
	    return false;
	}

	bool IsKeyUp(KeyCode key) const override
	{
		key;
	    return false;
	}

	bool IsKeyHeld(KeyCode key) const override
	{
		key;
	    return false;
	}

	math::Vec2 GetMousePosition() const override { return math::Vec2(0, 0) ; }

	bool IsMouseButtonDown(MouseButtonCode button) const override
	{
		button;
	    return false;
	}

	bool IsMouseButtonUp(MouseButtonCode button) const override
	{
		button;
	    return false;
	}

	bool IsMouseButtonHeld(MouseButtonCode button) const override
	{
		button;
	    return false;
	}

	bool IsSwitchButtonDown(SwitchInputs key) const override
	{
		key;
	    return false;
	}

	bool IsSwitchButtonUp(SwitchInputs key) const override
	{
		key;
	    return false;
	}

	bool IsSwitchButtonHeld(SwitchInputs key) const override
	{
		key;
	    return false;
	}

	bool IsControllerDown(ControllerInputs key) const override
	{
		key;
	    return false;
	}

	bool IsControllerUp(ControllerInputs key) const override
	{
		key;
	    return false;
	}

	bool IsControllerHeld(ControllerInputs key) const override
	{
		key;
	    return false;
	}

	bool IsActionDown(Action button) const override
	{
		button;
	    return false;
	}

	bool IsActionUp(Action button) const override
	{
		button;
	    return false;
	}

	bool IsActionHeld(Action button) const override
	{
		button;
	    return false;
	}

	void PrintJoystick(const int device) const override
	{
		device;
	}
	/**
	* \brief translate ActionCurrent enum to string
	*/
	std::string ActionEnumToString(Action action) override
	{
		action;
	    return "";
	}
	/**
	* \brief translate KeyCode enum to string
	*/
	std::string PcInputsEnumToString(KeyCode keyCode) override
	{
		keyCode;
	    return "";
	}
	std::string PcInputsEnumToString(MouseButtonCode mouseButton) override
	{
		mouseButton;
	    return "";
	}
	/**
	* \brief translate SwitchInputs enum to string
	*/
	std::string SwitchInputsEnumToString(SwitchInputs switchInputs) override
	{
		switchInputs;
	    return "";
	}
	/**
	* \brief translate ControllerInputs enum to string
	*/
	std::string ControllerInputsEnumToString(ControllerInputs controller) override
	{
		controller;
	    return "";
	}

    void SimulateKeyDown(KeyCode key) override
	{
		key;
	}
    void SimulateKeyUp(KeyCode key) override
	{
		key;
	}
    void SimulateMouseDown(MouseButtonCode button) override
	{
		button;
	}
    void SimulateMouseUp(MouseButtonCode button) override
	{
		button;
	}
    void SimulateActionDown(Action action) override
	{
		action;
	}
    void SimulateActionUp(Action action) override
	{
		action;
	}

    void Rumble(uint16_t lowFrequency, uint16_t highFrequency, uint32_t durationMS) override
	{
		lowFrequency;
		highFrequency;
		durationMS;
	}
};
} //namespace poke::input
