#include <Editor/Tools/input_binding_tool.h>

#include <imgui.h>

#include <CoreEngine/engine.h>

namespace poke {

namespace editor {

InputBindingTool::InputBindingTool(Editor& editor, const bool defaultActive) : Tool(defaultActive){
	isActive_ = defaultActive;
	name_ = "Input Binding Tool";
	toolFlag_ = TOOL_FLAG_INPUT_BINDING;
}

void InputBindingTool::OnDrawImgui(){
	ImGui::Begin(name_.c_str(), &isActive_);
	ImGui::Text("Input Binding Tool Test");
	ImGui::Text("Choose your actions and select the related inputs");
			
	if (ImGui::BeginCombo("Action", actionCurrent_)){
		for (int i = 0; i < size_t(actionsArray_.size()); i++){ 
			const bool isSelected = (actionCurrent_ == actionsArray_[i].c_str());
				
			if (ImGui::Selectable(actionsArray_[i].c_str(), isSelected)){
				actionCurrent_ = actionsArray_[i].c_str();
				indexAction_ = i;
				SetActionEnum(actionCurrent_);
				SetBindingsInput();
			}		
		}
		ImGui::EndCombo();
	}

	ImGui::BeginChild("ActionsBinding", ImVec2(0,90), true);
	{//Begin child
		if (ImGui::BeginCombo("Switch input", switchInputCurrent_)) {
			for (int i = 0; i < size_t(switchInputsArray_.size()); i++) { 
				const bool isSelected = (switchInputCurrent_ == switchInputsArray_[i].c_str());

				if (ImGui::Selectable(switchInputsArray_[i].c_str(), isSelected)) {
					switchInputCurrent_ = switchInputsArray_[i].c_str();
					indexSwitch_ = i;
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::BeginCombo("Controller input", controllerInputCurrent_)) {
			for (int i = 0; i < size_t(controllerInputsArray_.size()); i++) { 
				const bool isSelected = (controllerInputCurrent_ == controllerInputsArray_[i].c_str()); 

				if (ImGui::Selectable(controllerInputsArray_[i].c_str(), isSelected)) {
					controllerInputCurrent_ = controllerInputsArray_[i].c_str();
					indexController_ = i;
				}

			}
			ImGui::EndCombo();
		}
		if (ImGui::BeginCombo("PC input", pcInputCurrent_)) {
			for (int i = 0; i < size_t(pcInputsArray_.size()); i++) {
				const bool isSelected = (pcInputCurrent_ == pcInputsArray_[i].c_str());

				if (ImGui::Selectable(pcInputsArray_[i].c_str(), isSelected)) {
					pcInputCurrent_ = pcInputsArray_[i].c_str();
					indexPc_ = i;
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::BeginCombo("Switch input", switchInputCurrent_)) {
			for (int i = 0; i < static_cast<int>(switchInputsArray_.size()); i++) {
				bool isSelected = (switchInputCurrent_ == switchInputsArray_[i].c_str());

				if (ImGui::Selectable(switchInputsArray_[i].c_str(), isSelected)) {
					switchInputCurrent_ = switchInputsArray_[i].c_str();
					indexSwitch_ = i;
				}
			}
			ImGui::EndCombo();
		}
	}//End child
	ImGui::EndChild();

	if (ImGui::Button("Bind input"))ImGui::OpenPopup("bind");
		
	if (ImGui::BeginPopup("bind")){
		Bind();
		ImGui::Text("Input bind");
		ImGui::EndPopup();
	}
	if (ImGui::Button("Load From Json"))ImGui::OpenPopup("load from json");
		
	if (ImGui::BeginPopup("load from json")){
		LoadBindingsFromJson();
		SetBindingsInput();
		ImGui::Text("loaded from json");
		ImGui::EndPopup();
	}
	ImGui::End();
}

void InputBindingTool::OnSetActive(){
	if (!PokFileSystem::IsDirectory(PokFileSystem::GetPath(FileType::BINDING, FolderType::SAVE))) {
		PokFileSystem::CreateDirectory(PokFileSystem::GetPath(FileType::BINDING, FolderType::SAVE));
	}

	SetArraysOfInputs();
	bindingFromJson_.resize(static_cast<int>(input::Action::LENGTH));
	for (int i = 0; i < static_cast<int>(input::Action::LENGTH); i++){
		bindingFromJson_[i].emplace("Actions", 0);
		bindingFromJson_[i].emplace("PcInput", 0);
		bindingFromJson_[i].emplace("SwitchInput", 0);
		bindingFromJson_[i].emplace("ControllerInput", 0);
	}
	LoadBindingsFromJson();
	SetBindingsInput();
}

void InputBindingTool::OnSetInactive(){
	actionsArray_.clear();
	pcInputsArray_.clear();
	switchInputsArray_.clear();
	controllerInputsArray_.clear();
	bindingFromJson_.clear();
}

void InputBindingTool::CreateToJson(json data) const {
	data[static_cast<int>(action_)] = bindingFromJson_[static_cast<int>(action_)];
	PokFileSystem::WriteFile("input_json", data, FileType::BINDING);
}
void InputBindingTool::SetActionEnum(const std::string &string) {
	if (string == "Up")action_ = input::Action::UP;
	
	if (string == "Down")action_ = input::Action::DOWN;

	if (string == "Left")action_ = input::Action::LEFT;

	if (string == "Right")action_ = input::Action::RIGHT;
	
	if (string == "Dash")action_ = input::Action::DASH;
	
	if (string == "Main_Shoot")action_ = input::Action::MAIN_SHOOT;
	
	if (string == "Secondary_Target")action_ = input::Action::SECONDARY_TARGET;
	
	if (string == "Secondary_Shoot")action_ = input::Action::SECONDARY_SHOOT;
	
	if (string == "Secondary_cancel")action_ = input::Action::SECONDARY_CANCEL;
	
	if (string == "Abilities")action_ = input::Action::ABILITIES;
	
}

	
void InputBindingTool::SetArraysOfInputs(){

	auto& inputManager = InputManagerLocator::Get();
	actionsArray_.reserve(static_cast<int>(input::Action::LENGTH));
	for (int i = 0; i != static_cast<int>(input::Action::LENGTH); i++){
		actionsArray_.emplace_back(inputManager.ActionEnumToString(static_cast<input::Action>(i)));
	}
	actionCurrent_ = actionsArray_[0].c_str();


	switchInputsArray_.reserve(static_cast<int>(input::SwitchInputs::LENGTH));
	for (int i = 0; i != static_cast<int>(input::SwitchInputs::LENGTH); i++){
		switchInputsArray_.emplace_back(inputManager.SwitchInputsEnumToString(static_cast<input::SwitchInputs>(i)));
	}
	switchInputCurrent_ = switchInputsArray_[0].c_str();
			
	controllerInputsArray_.reserve(static_cast<int>(input::ControllerInputs::LENGTH));
	for (int i = 0; i != static_cast<int>(input::ControllerInputs::LENGTH); i++){
		controllerInputsArray_.emplace_back(inputManager.ControllerInputsEnumToString(static_cast<input::ControllerInputs>(i)));
	}
	controllerInputCurrent_ = controllerInputsArray_[0].c_str();

	pcInputsArray_.reserve(static_cast<int>(input::KeyCode::KEYBOARD_SIZE));
	for (int i = 0; i != static_cast<int>(input::KeyCode::KEYBOARD_SIZE); i++){
		auto inputName = inputManager.PcInputsEnumToString(static_cast<input::KeyCode>(i));
		pcInputsArray_.emplace_back(inputName);
	}
	for (int i = 0; i < static_cast<int>(input::MouseButtonCode::LENGTH); i++) {
		pcInputsArray_.emplace_back(inputManager.PcInputsEnumToString(static_cast<input::MouseButtonCode>(i)));
	}
	pcInputCurrent_ = pcInputsArray_[0].c_str();
}
void InputBindingTool::LoadBindingsFromJson(){
	json tmpJson = PokFileSystem::ReadJsonFile("input_json", FileType::BINDING, FolderType::SAVE);
	for (int i = 0; i < static_cast<int>(input::Action::LENGTH); i++){
		if (tmpJson[i] != nullptr){
			bindingFromJson_[i]["Actions"] = tmpJson[i]["Actions"];
			bindingFromJson_[i]["PcInput"] = tmpJson[i]["PcInput"];
			bindingFromJson_[i]["SwitchInput"] = tmpJson[i]["SwitchInput"];
			bindingFromJson_[i]["ControllerInput"] = tmpJson[i]["ControllerInput"];
		}
	}
}
void InputBindingTool::Bind(){
	bindingFromJson_[static_cast<int>(action_)]["Actions"] = indexAction_;
	bindingFromJson_[static_cast<int>(action_)]["PcInput"] = indexPc_;
	bindingFromJson_[static_cast<int>(action_)]["SwitchInput"] = indexSwitch_;
	bindingFromJson_[static_cast<int>(action_)]["ControllerInput"] = indexController_;
	CreateToJson(PokFileSystem::ReadJsonFile("input_json", FileType::BINDING, FolderType::SAVE)); 
}

void InputBindingTool::SetBindingsInput(){
	indexPc_ = bindingFromJson_[static_cast<int>(action_)]["PcInput"];
	indexSwitch_ = bindingFromJson_[static_cast<int>(action_)]["SwitchInput"];
	indexController_ = bindingFromJson_[static_cast<int>(action_)]["ControllerInput"];
	pcInputCurrent_ = pcInputsArray_[indexPc_].c_str();
	switchInputCurrent_ = switchInputsArray_[indexSwitch_].c_str();
	controllerInputCurrent_ = controllerInputsArray_[indexController_].c_str();
}
} //namespace editor
} //namespace poke