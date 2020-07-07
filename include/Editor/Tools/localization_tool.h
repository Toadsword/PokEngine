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
// Author : Simon Canas
// Co-Author :
// Date : 09.12.19
//----------------------------------------------------------------------------------
#pragma once

#include <string>
#include <vector>

#include <json.hpp>
#include <imgui.h>

#include <Editor/Tools/tool.h>
#include <Utility/file_system.h>

namespace poke::editor
{
class Editor;

/**
 * \brief A representation of a term
 */
struct Term
{
	std::string name = ""; 
	bool isFullyTranslated = true;
};

enum class Languages : uint8_t
{
	EN_US = 0,
	FR_FR,
	ES_ES,
	DE_DE,
	IT_IT,
	LENGTH
};

inline std::string LanguagesToLocale(const Languages& language)
{
	switch (language)
	{
	default: return "EN_US";
	case Languages::EN_US: return "EN_US";
	case Languages::FR_FR: return "FR_FR";
	case Languages::ES_ES: return "ES_ES";
	case Languages::DE_DE: return "DE_DE";
	case Languages::IT_IT: return "IT_IT";
	}
}

inline Languages LocaleToLanguages(const std::string& locale)
{
	if (locale == "EN_US") return Languages::EN_US;
	if (locale == "FR_FR") return Languages::FR_FR;
	if (locale == "ES_ES") return Languages::ES_ES;
	if (locale == "DE_DE") return Languages::DE_DE;
	if (locale == "IT_IT") return Languages::IT_IT;

	return Languages::EN_US;
}

/**
* \brief Allows the engine to handle multiple languages
*/
class LocalizationTool final : public Tool
{
public:
	explicit LocalizationTool(Editor& editor, bool defaultActive = false);

	/**
	 * \brief Is called when the tool is opened
	 */
	void OnSetActive() override;

	/**
	 * \brief Is called when the tool is closed
	 */
	void OnSetInactive() override;

	/**
	 * \brief Sets the game's language
	 * \param language the language to change to
	 */
	void SetGameLanguage(const Languages& language);

	/**
	 * \brief Finds the language file 'languageName_ + ".poklang"' in 'langFilesLocation_' and finds the term with its associated string
	 * \param termName the term to look up in the configured language
	 * \return The string associated with 'termName'. Returns '?termName?' if file not found, and '!termName!' if term not found
	 */
	std::string GetTermValue(const std::string& termName) const;


	/**
	 * \brief Inserts a string in a sorted vector while keeping it sorted
	 * \param vector the sorted vector to insert in
	 * \param insertVal the value to insert
	 * \return The index to where the value was inserted
	 */
	unsigned int InsertTermInSortedVector(std::vector<Term>& vector, const Term& insertVal);

	/**
	 * \brief Renders a small window for testing purposes
	 */
	void OnDrawImgui() override;
private:
	//-----------------------------------------------------------------------------
	// Tool
	//-----------------------------------------------------------------------------
	const char* langListName_ = "lang_list"; //The name of the file containing the list of languages 
	json langList_;  //The json containing the list of languages

	Languages currentGameLanguage_ = Languages::EN_US;
	std::vector<std::string> languageLocalesList_;

	std::vector<json> langFiles_; //List of all json files

	//First element of the pairs contains the term name
	//Second element of the pairs tells whether the term is fully translated or not
	std::vector<Term> termList_;

	//-----------------------------------------------------------------------------
	// ImGui interface
	//-----------------------------------------------------------------------------
	/**
	 * \brief Creates the term selection box and search bar
	 */
	void TermSelection();

	/**
	 * \brief Creates the "New Term..." button and popup
	 */
	void NewTerm();

	/**
	 * \brief Creates the language SubTree with all translations
	 */
	void LanguagesSubTree();

	/**
	 * \brief Creates the "Delete Term..." button and popup
	 */
	void DeleteTerm();

	/**
	 * \brief Saves terms to files and updates "isTermFullyTranslated_"
	 */
	void SaveTerms();

	/**
     * \brief Saves all terms and languages to a csv file
     */
	void ExportToCsv();

	/**
	 * \brief Updates "multilineHeights_"
	 */
	void UpdateMultilineHeight();

	std::string currentLangCombo_; //Which language is currently selected in ImGui
	unsigned int currentTerm_ = 0; //Which term is currently selected in ImGui
	ImGuiTextFilter searchFilter_; //Filter used for the search bar
	std::vector<std::string> termBuffers_; //Buffers for ImGui's InputText()
	std::string newLocaleBuffer_; //For the InputText() when "Add Language..." is pressed
	std::string newLangBuffer_; //For the InputText() when "Add Language..." is pressed
	std::string newNameBuffer_; //For the InputText() when "Create Term..." is pressed

	bool isMultiline_ = false;
	std::vector<float> multilineHeights_; //Size of multiline

	const float kLineHeight_ = 22; //Size of multiline
	const float kSelectionHeight_ = 20 * 6; //Selection box max height
	const char kCsvDelimiter_ = ';';
	const ImVec2 kButtonSize_ = ImVec2(100, 0); //Size of buttons in DeleteTerm()
	const ImVec4 kWhiteCol_ = ImVec4(1, 1, 1, 1); //The color white
	const ImVec4 kRedCol_ = ImVec4(1, 0, 0, 1); //The color red
};
} //namespace poke::editor
