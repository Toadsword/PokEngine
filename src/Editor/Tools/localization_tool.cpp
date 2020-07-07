#include <Editor/Tools/localization_tool.h>

#include <imgui.h>
#include <SDL.h>
#include <misc/cpp/imgui_stdlib.h> 

#include <Utility/json_utility.h>

namespace poke::editor
{
//-----------------------------------------------------------------------------
// Tool
//-----------------------------------------------------------------------------
LocalizationTool::LocalizationTool(Editor& editor, const bool defaultActive) :
	Tool(defaultActive) {
	isActive_ = defaultActive;
	name_ = "Localization Tool";
	toolFlag_ = TOOL_FLAG_LOCALIZATION;
}

void LocalizationTool::OnSetActive() {
	langList_ = LoadJson(langListName_, FileType::LANG);
	currentGameLanguage_ = LocaleToLanguages(langList_["currentLang"]);
	termBuffers_.resize(static_cast<int>(Languages::LENGTH));
	multilineHeights_.resize(static_cast<int>(Languages::LENGTH));

	//Generates an array with the languages names to display in ImGui
	for (int i = 0; i < static_cast<int>(Languages::LENGTH); ++i) {
		languageLocalesList_.push_back(LanguagesToLocale(static_cast<Languages>(i)));
		langFiles_.push_back(LoadJson(
			LanguagesToLocale(static_cast<Languages>(i)), FileType::LANG));

		termBuffers_.emplace_back("");

		multilineHeights_.push_back(kLineHeight_);
	}

	const std::string curLang = langList_[LanguagesToLocale(currentGameLanguage_)];
	currentLangCombo_ = curLang;

	int index = 0;
	//Creates a list containing every terms
	for (auto& term : langFiles_[static_cast<int>(currentGameLanguage_)].items()) {
		termList_.emplace_back(Term{term.key()});

		//Checks if the term is translated in every language
		for (auto& file : langFiles_) {
			//For some reason .empty() doesn't work
			if (file[term.key()] == "")	{
				termList_[index].isFullyTranslated = false;
				break;
			}
		}

		index++;
	}
}

void LocalizationTool::OnSetInactive() {
	langFiles_.clear();
	termList_.clear();
	termBuffers_.clear();
	multilineHeights_.clear();
}

void LocalizationTool::SetGameLanguage(const Languages& language) {
	currentGameLanguage_ = language;
}

std::string LocalizationTool::GetTermValue(const std::string& termName) const {
	json langJson = langFiles_[static_cast<int>(currentGameLanguage_)];

	//Check if term exists
	if (CheckJsonExists(langJson, termName)) {   
		return langJson[termName];
	}

	//Error handling
	return "!" + termName + "!";
}

//-----------------------------------------------------------------------------
// ImGui interface
//-----------------------------------------------------------------------------
void LocalizationTool::OnDrawImgui() {
	ImGui::Begin(name_.c_str(), &isActive_);

	ImGui::Text("%s", GetTermValue("langText").c_str());

    if(ImGui::Button("Export to csv")) {
		ExportToCsv();
    }

	ImGui::Separator();

	//Custom combo for selecting the game language
	if (ImGui::BeginCombo("Language", currentLangCombo_.c_str())) {
		for (size_t i = 0; i < languageLocalesList_.size(); ++i) {
			bool isSelected = static_cast<size_t>(currentGameLanguage_) == i;
			std::string label = langList_[LanguagesToLocale(static_cast<Languages>(i))];

			if (ImGui::Selectable(label.c_str(), &isSelected)) {
				SetGameLanguage(static_cast<Languages>(i));
				currentLangCombo_ = label;
			}
		}

		ImGui::EndCombo();
	}

	ImGui::Separator();

	TermSelection();

	LanguagesSubTree();

	ImGui::End();
}

void LocalizationTool::TermSelection() {
	ImGui::Text("Terms");

	//Draws the search bar
	searchFilter_.Draw("Search");

	//Custom selection box for more flexibility
	ImGui::BeginChildFrame(ImGui::GetID("Terms"), ImVec2(0, kSelectionHeight_));

	//Create an entry for each term
	for (size_t i = 0; i < termList_.size(); ++i) {
		//Checks if filter includes the term
		if (!searchFilter_.PassFilter(termList_[i].name.c_str())) continue;

		//Makes the term selectable
		bool isSelected = currentTerm_ == i;
		if (ImGui::Selectable(("##" + termList_[i].name).c_str(), &isSelected))	{
			SaveTerms();
			currentTerm_ = i;

			isMultiline_ = false;
		}

		//Superposes Selectable() and TextColored()
		//Because there is no other way to make colored text
		ImGui::SameLine(0, 0);

		//Makes the term name red if a translation is missing
		const ImVec4 textCol = termList_[i].isFullyTranslated ? kWhiteCol_ : kRedCol_;
		ImGui::TextColored(textCol, "%s", termList_[i].name.c_str());
	}

	ImGui::EndChildFrame();

	NewTerm();

	ImGui::Separator();

	const ImVec4 textCol = termList_[currentTerm_].isFullyTranslated ? kWhiteCol_ : kRedCol_;
	ImGui::Text("%s", "Selected term:");
	ImGui::SameLine();
	ImGui::TextColored(textCol, "%s", termList_[currentTerm_].name.c_str());
}

void LocalizationTool::NewTerm(){
	if (ImGui::Button("New Term...")) newNameBuffer_ = "";

	//Creates the popup
	if (ImGui::BeginPopupContextItem("New Term", ImGui::GetIO().MouseClicked[0])){
		ImGui::InputTextWithHint(
			"Name", 
			"Enter a name for the term",
			&newNameBuffer_
		);
		if (!newNameBuffer_.empty() and ImGui::Button("Create")) {
			//Makes the first character lowercase so it can be sorted correctly in the json
			newNameBuffer_[0] = tolower(newNameBuffer_[0]);

			for (size_t i = 0; i < langFiles_.size(); ++i) {
				//Write the new term to all lang files
				langFiles_[i][newNameBuffer_] = "";
				PokFileSystem::WriteFile(
					LanguagesToLocale(static_cast<Languages>(i)), langFiles_[i],
					FileType::LANG
				);

				//Updates the buffers to the new term
				//Can't convert that to 'const char*' due to how nlohmann json works
				const std::string newTerm = langFiles_[i][newNameBuffer_];
				termBuffers_[i] = newTerm;
			}

			//Inserts the new term into the list, while keeping the list sorted
			Term newTerm;
			newTerm.name = newNameBuffer_;
			currentTerm_ = InsertTermInSortedVector(termList_, newTerm);

			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void LocalizationTool::LanguagesSubTree()
{
	//A checkbox to check if the term value is multiline (e.g.: for dialogues/long text...)
	if (ImGui::Checkbox("Multiline input?", &isMultiline_) && isMultiline_)	{
		UpdateMultilineHeight();
	}

	for (size_t i = 0; i < langFiles_.size(); ++i) {
		//Gets the json key
		const std::string nextTerm = langFiles_[i][termList_[currentTerm_].name];
		termBuffers_[i] = nextTerm;

		//Sets the json key
		ImGui::Bullet();

		const std::string label = langList_[LanguagesToLocale(static_cast<Languages>(i))];
		if (isMultiline_) {
			ImGuiIO& io = ImGui::GetIO();
			std::string lastBuf;
			if (!termBuffers_[i].empty()) lastBuf = termBuffers_[i].substr(
				termBuffers_[i].length() - 1);

			if (ImGui::InputTextMultiline(
				    label.c_str(),
				    &termBuffers_[i],
				    ImVec2(0, multilineHeights_[i])
			        )
				) {
				if (io.KeysDown[SDL_SCANCODE_RETURN]) {
					multilineHeights_[i] = multilineHeights_[i] + kLineHeight_;
				}
				if (io.KeysDown[SDL_SCANCODE_BACKSPACE] && lastBuf == "\n") {   
					multilineHeights_[i] = multilineHeights_[i] - kLineHeight_;
				}

				langFiles_[i][termList_[currentTerm_].name] = termBuffers_[i];
			}
		} else {
			if (ImGui::InputText(label.c_str(), &termBuffers_[i])) {
				langFiles_[i][termList_[currentTerm_].name] = termBuffers_[i];
			}
		}

		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("%s", termBuffers_[i].c_str());
		}
	}

	if (ImGui::Button("Save...")) SaveTerms();

	DeleteTerm();
}

void LocalizationTool::SaveTerms() {
	unsigned int counter = 0;
	for (size_t i = 0; i < langFiles_.size(); ++i) {
		if (langFiles_[i][termList_[currentTerm_].name] != "") {
			counter++;
		}
		PokFileSystem::WriteFile(LanguagesToLocale(static_cast<Languages>(i)), langFiles_[i], FileType::LANG);
	}

	if (counter == langFiles_.size()) {
		termList_[currentTerm_].isFullyTranslated = true;
	} else {
		termList_[currentTerm_].isFullyTranslated = false;
	}
}

void LocalizationTool::ExportToCsv() {

    //Setting up header
	std::string output = "";
	for (size_t i = 0; i < langFiles_.size(); ++i) {
		output += "\"" + LanguagesToLocale(static_cast<Languages>(i)) + "\"" + kCsvDelimiter_;
	}
    //End of line
	output += "\r\n";

    //Registering terms
    for(size_t iTerm = 0; iTerm < termList_.size(); iTerm++) { 
		for (size_t iLang = 0; iLang < langFiles_.size(); iLang++) {
            std::string trad = langFiles_[iLang][termList_[iTerm].name];
			output += "\"" + trad + "\"" + kCsvDelimiter_;
		}

		//End of line
		output += "\n";
    }

	PokFileSystem::WriteFile("exportCsvLang", output, FileType::LANG_CSV);
}

void LocalizationTool::DeleteTerm() {
	//Only display the button when there is at least 2 terms
	if (termList_.size() > 1 and ImGui::Button("Delete Term...")) {
		ImGui::OpenPopup("Delete Term");
	}

	if (ImGui::BeginPopupModal(
		"Delete Term", 
		nullptr,
		ImGuiWindowFlags_AlwaysAutoResize)
	)	{
		ImGui::Text("%s", ("Delete \"" + termList_[currentTerm_].name + "\"?").c_str());
		ImGui::Separator();

		if (ImGui::Button("OK", kButtonSize_)) {
			for (size_t i = 0; i < langFiles_.size(); ++i) {
				langFiles_[i].erase(termList_[currentTerm_].name);
				PokFileSystem::WriteFile(LanguagesToLocale(static_cast<Languages>(i)), langFiles_[i],
					FileType::LANG);

				//To prevent crashes if the first term is deleted
				const std::string newActiveTerm = termList_[currentTerm_].name !=
					termList_[0].name ?
					langFiles_[i][termList_[0].name] :
					langFiles_[i][termList_[1].name];

				//Updates the buffers
				termBuffers_[i] = newActiveTerm;
			}

			//Erases the currently selected term
			termList_.erase(termList_.begin() + currentTerm_);

			//To prevent crashes if the first term is deleted
			currentTerm_ = static_cast<int>(currentTerm_) - 1 >= 0
				? currentTerm_ - 1 : 0;

			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", kButtonSize_)) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void LocalizationTool::UpdateMultilineHeight()
{
	for (size_t j = 0; j < termBuffers_.size(); ++j)
	{
		const float lineBreakNum = std::count(termBuffers_[j].begin(),
			termBuffers_[j].end(), '\n');
		multilineHeights_[j] = kLineHeight_ * (lineBreakNum + 1);
	}
}

unsigned int LocalizationTool::InsertTermInSortedVector(
	std::vector<Term>& vector, 
	const Term& insertVal
) {
	for (size_t i = 0; i < vector.size(); ++i) {
		//Optimization: skips if first char of string is inferior
		if (vector[i].name[0] >= insertVal.name[0])	{
			//Cycles through each char of string
			for (size_t j = 0; j < vector[i].name.size(); ++j) {
				//Allows to break early
				if (j >= insertVal.name.size() or vector[i].name[j] > insertVal.name[j]) {
					vector.insert(vector.begin() + i, insertVal);
					currentTerm_ = i;
					return i;
				}
			}
		}
	}

	vector.emplace_back(insertVal);
	return vector.size() - 1;
}
} //namespace poke::editor
