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
// Date : 06.02.2020
//----------------------------------------------------------------------------------
#pragma once

#include <Editor/Tools/tool.h>
#include <Utility/log.h>

namespace poke::editor {
class Editor;

/**
 * \brief Tool that displays all the logs in the console
 */
class LogTool : public Tool {
public:
	explicit LogTool(Editor& editor, bool defaultActive = false);

	void OnDrawImgui() override; 

    void OnSetInactive() override;
    void OnSetActive() override;

private:
	bool DisplayMenuItem(LogType logType);

	unsigned ManageScrollBar(unsigned minIndex, const unsigned numLogs, const unsigned maxRowsDisplayed); 

	const float kPaddingX_ = 25.0f;
	const float kPaddingY_ = 60.0f;

	size_t lastNumLogs_ = 0;
	long long currentStoppedIndex_ = 0;
	long long lastClearAt_ = 0;
	float percentScrollBar_ = 0.0f; 
	float lastPercentScrollBar_ = 0.0f;

    //Severity
	bool displayDebug_ = true;
	bool displayWarning_ = true;
	bool displayError_ = true;

    //Type
	uint8_t logTypeFlag_ = static_cast<uint8_t>(LogType::ALL_LOG);
};
} //namespace poke::editor