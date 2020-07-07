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
// Co-Author : Luca Floreau
// Date : 25.02.20
//----------------------------------------------------------------------------------
#pragma once

#include <vector>

#include <CoreEngine/module.h>
#include <Math/vector.h>

#include <imgui.h>
#include <SDL.h>

#include <CoreEngine/Observer/subjects_container.h>
#include <Inputs/interface_input_manager.h>

namespace poke {
//-----------------------------FORWARD DECLARATION----------------------------------
class Engine;

//----------------------------------------------------------------------------------

/**
 * \brief Manage inputs
 */
namespace input {
class InputManager final : public IInputManager {
public:
    explicit InputManager(
        Engine& engine);

    void OnAppBuild() override;

    void BindFromJson() override;

    void OnPreUserInputs() override;

    bool IsKeyDown(KeyCode key) const override;

    bool IsKeyUp(KeyCode key) const override;

    bool IsKeyHeld(KeyCode key) const override;

    bool IsSwitchButtonDown(SwitchInputs key) const override;

    bool IsSwitchButtonUp(SwitchInputs key) const override;

    bool IsSwitchButtonHeld(SwitchInputs key) const override;

    bool IsControllerDown(ControllerInputs key) const override;

    bool IsControllerUp(ControllerInputs key) const override;

    bool IsControllerHeld(ControllerInputs key) const override;

    math::Vec2 GetMousePosition() const override;

    bool IsMouseButtonDown(MouseButtonCode button) const override;

    bool IsMouseButtonUp(MouseButtonCode button) const override;

    bool IsMouseButtonHeld(MouseButtonCode button) const override;

    bool IsActionDown(Action button) const override;

    bool IsActionUp(Action button) const override;

    bool IsActionHeld(Action button) const override;

    void PrintJoystick(int device) const override;

    std::string ActionEnumToString(Action action) override;

    std::string PcInputsEnumToString(KeyCode keyCode ) override;
	std::string PcInputsEnumToString(MouseButtonCode mouseButton) override;

    std::string SwitchInputsEnumToString(SwitchInputs switchInputs) override;

    std::string ControllerInputsEnumToString(ControllerInputs controller) override;

    void SimulateKeyDown(KeyCode key) override;

	void SimulateKeyUp(KeyCode key) override;

	void SimulateMouseDown(MouseButtonCode button) override;

	void SimulateMouseUp(MouseButtonCode button) override;

	void SimulateActionDown(Action action) override;

	void SimulateActionUp(Action action) override;

    void Rumble(uint16_t lowFrequency, uint16_t highFrequency, uint32_t durationMS) override;

    inline static int scrollX = 0;
    inline static int scrollY = 0;
    inline static int scrollUpdate = false;

    inline static float joyAxisLX = 0;
    inline static float joyAxisLY = 0;
    inline static float joyAxisRX = 0;
    inline static float joyAxisRY = 0;
    inline static int joyAxisUpdate = false;

    /**
	 * \brief Boolean variable set in true if the last input is from a controller, false if from a mouse/keyboard.
	 */
	inline static bool usingController = false;

protected:
    std::vector<int> bindingPcInput_ =
        std::vector<int>(static_cast<int>(Action::LENGTH));
    std::vector<int> bindingSwitchInput_ =
        std::vector<int>(static_cast<int>(Action::LENGTH));
    std::vector<int> bindingControllerInput_ =
        std::vector<int>(static_cast<int>(Action::LENGTH));

    std::vector<ButtonState> keyPressedState_ =
        std::vector<ButtonState>(static_cast<int>(KeyCode::KEYBOARD_SIZE));
    std::vector<ButtonState> buttonState_ =
        std::vector<ButtonState>(static_cast<int>(MouseButtonCode::MOUSE_MAX));
    std::vector<ButtonState> switchButtonState_ =
        std::vector<ButtonState>(static_cast<int>(SwitchInputs::LENGTH));
    std::vector<ButtonState> controllerButtonState_ =
        std::vector<ButtonState>(static_cast<int>(ControllerInputs::LENGTH));

    const uint8_t* keyboard_;
    uint32_t mouse_;
    math::Vec2 mousePos_;

    SDL_Joystick* joystick_{};
    SDL_GameController* controller_{};

    struct ImGuiContext* imGuiContext_ = nullptr;

	const float kMaxJoyValue_ = 32768.0f;

    Engine& engine_;
};
} //namespace input
} //namespace poke
