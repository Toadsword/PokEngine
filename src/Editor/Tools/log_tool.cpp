#include <Editor/Tools/log_tool.h>
#include <imgui.h>

#include <CoreEngine/engine.h>
#include <Editor/editor_utility.h>
#include <Utility/color.h>
#include <Utility/log.h>

namespace poke::editor {
LogTool::LogTool(Editor& editor, const bool defaultActive) : Tool(defaultActive)
{
	isActive_ = defaultActive;
	name_ = "Log tool";
	toolFlag_ = TOOL_FLAG_LOG;
}

void LogTool::OnDrawImgui() 
{
	const auto& logs = GetLogs();
	const auto& logsType = GetTypeLogs();
	const auto& logsSeverity = GetSeverityLogs();
	const auto numLogs = GetNumLogs();

	if (ImGui::Begin("Logs", &isActive_, ImGuiWindowFlags_MenuBar)) {
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("Severity")) {
				if (ImGui::MenuItem("Display debugs", "", displayDebug_)) {
					displayDebug_ = !displayDebug_;
				}
				if (ImGui::MenuItem("Display warnings", "", displayWarning_)) {
					displayWarning_ = !displayWarning_;
				}
				if (ImGui::MenuItem("Display errors", "", displayError_)) {
					displayError_ = !displayError_;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Type")) {
				bool allDisplayed = true;
				allDisplayed = DisplayMenuItem(LogType::DEFAULT_LOG) && allDisplayed;
				allDisplayed = DisplayMenuItem(LogType::GRAPHICS_LOG) && allDisplayed;
				allDisplayed = DisplayMenuItem(LogType::PHYSICS_LOG) && allDisplayed;
				allDisplayed = DisplayMenuItem(LogType::INPUT_LOG) && allDisplayed;
				allDisplayed = DisplayMenuItem(LogType::ECS_LOG) && allDisplayed;
				allDisplayed = DisplayMenuItem(LogType::AUDIO_LOG) && allDisplayed;
				allDisplayed = DisplayMenuItem(LogType::TOOL_LOG) && allDisplayed;
				allDisplayed = DisplayMenuItem(LogType::EDITOR_LOG) && allDisplayed;
				allDisplayed = DisplayMenuItem(LogType::SCENE_LOG) && allDisplayed;
				if (ImGui::MenuItem(
					Logger::Get().GetLogTypeString(LogType::ALL_LOG),
					"",
					allDisplayed)) {
					if (allDisplayed) {
						logTypeFlag_ = 0;
					} else {
						logTypeFlag_ = static_cast<uint8_t>(LogType::ALL_LOG);
					}
				}
				ImGui::EndMenu();
			}

            if(ImGui::Button("Clear")) {
				lastClearAt_ = numLogs - 1;
            }
			ImGui::EndMenuBar();
		}
	    
		unsigned maxRowDisplayed = 
			(ImGui::GetWindowContentRegionMax().y - kPaddingY_) /
			(ImGui::GetFrameHeight() - 1);
		unsigned minIndex = numLogs > maxRowDisplayed ? numLogs - maxRowDisplayed : 0;

		minIndex = ManageScrollBar(minIndex, numLogs, maxRowDisplayed);
		if (maxRowDisplayed > numLogs) {
			maxRowDisplayed = numLogs;
		}
		//Display log window
		ImGui::BeginChild(
			"scrolling", 
			ImVec2(
				ImGui::GetWindowContentRegionMax().x - kPaddingX_, 
				ImGui::GetWindowContentRegionMax().y - kPaddingY_
			), 
			false, 
			ImGuiWindowFlags_HorizontalScrollbar
		);
		{ //Begin Child
		    unsigned displayed = 0;
			const unsigned constMaxRowDisplayed = maxRowDisplayed;
		    for(unsigned i = minIndex; 
				i - lastClearAt_ > 0 && displayed < constMaxRowDisplayed; 
				i--) 
			{
                if(!(static_cast<uint8_t>(logsType[i]) & logTypeFlag_) && 
					logsType[i] != LogType::ALL_LOG) {
					continue;
                }

			    switch(logsSeverity[i]) {
			        case LogSeverity::ERROR:
				        if (displayError_) {
					        ImGui::TextColored(
								PokToImGuiColor(Color::red), 
								logs[i].c_str()
							);
					        displayed++;
				        }
				        break;
			        case LogSeverity::WARNING:
				        if (displayWarning_) {
					        ImGui::TextColored(
								PokToImGuiColor(Color::orange), 
								logs[i].c_str()
							);
					        displayed++;
				        }
				        break;
				    case LogSeverity::DEBUG:
                    default:
					    if (displayDebug_) {
						    ImGui::Text(logs[i].c_str());
						    displayed++;
					    }
					    break;
			    }
		    }
			ImGui::EndChild();
		} // End Child
		
		//Display vertical scrollbar
		ImGui::SameLine();
		ImGui::VSliderFloat(
			"##logScrollV", 
			ImVec2(10, ImGui::GetWindowContentRegionMax().y - kPaddingY_),
			&percentScrollBar_, 
			1, 
			0
		);
	}
	ImGui::End();
}

unsigned LogTool::ManageScrollBar(
	unsigned minIndex, 
	const unsigned numLogs, 
	const unsigned maxRowsDisplayed)
{
    // When we update the scrollbar, we change the index of the scroll bar
	if (percentScrollBar_ != lastPercentScrollBar_) {
		currentStoppedIndex_ = percentScrollBar_ * numLogs;
	}
	else {
		//Prevent autoscrolling when consulting older messages
		currentStoppedIndex_ += (numLogs - lastNumLogs_);
		if (percentScrollBar_ > 0.0f) {
			percentScrollBar_ = 1.0f / 
				(static_cast<float>(numLogs) / currentStoppedIndex_);
		}
	}

	//Scroll through the mouse.
	currentStoppedIndex_ -= input::InputManager::scrollY;

	//Activate scrolling even though we're in listing of all logs.
	if (
		ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows) && 
		input::InputManager::scrollY < 0 && percentScrollBar_ == 0.0f
	) {
		currentStoppedIndex_ = 1;
		percentScrollBar_ = 
			1.0f / (static_cast<float>(numLogs) / currentStoppedIndex_);
	}

	// Check boundaries of list.
	if (currentStoppedIndex_ < 0) {
		currentStoppedIndex_ = 0;
	}
	if (currentStoppedIndex_ > numLogs) {
		currentStoppedIndex_ = numLogs;
	}

	// Prevent autoscrolling when consulting older messages.
	if (percentScrollBar_ > 0.0f) {
		minIndex -= minIndex >= currentStoppedIndex_ - maxRowsDisplayed ? 
			currentStoppedIndex_ - maxRowsDisplayed : 0;
	} else {
		minIndex = numLogs - 1;
	}

	// Update local infos.
	lastPercentScrollBar_ = percentScrollBar_;
	lastNumLogs_ = numLogs;

	return minIndex;
}

void LogTool::OnSetInactive() {}
void LogTool::OnSetActive() {}
bool LogTool::DisplayMenuItem(LogType logType)
{
	const bool displayed = logTypeFlag_ & static_cast<uint8_t>(logType);
	if (ImGui::MenuItem(
		Logger::Get().GetLogTypeString(logType),
		"", 
		displayed)) {
		if (displayed) {
			logTypeFlag_ -= static_cast<uint8_t>(logType);
		} else {
			logTypeFlag_ |= static_cast<uint8_t>(logType);
		}
	}
	return displayed;
}
} //namespace poke::editor
