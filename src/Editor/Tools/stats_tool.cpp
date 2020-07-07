#include <Editor/Tools/stats_tool.h>

#include <iostream>
#include <string>
#include <imgui.h>

#include <Utility/time_custom.h>


namespace poke::editor{
StatsTool::StatsTool(Editor& editor, const bool defaultActive) :
    Tool(defaultActive) 
{
    isActive_ = defaultActive;
    name_ = "Tool Stats";
	toolFlag_ = TOOL_FLAG_STATS;
}

void StatsTool::OnDrawImgui() 
{
    ImGui::Begin(name_.c_str(), &isActive_);

	framesCount_ += 1;

	ImGui::Text("Passed frames %s", std::to_string(framesCount_).c_str());
	ImGui::Text(
		"%s fps", 
		std::to_string(1.0f / Time::Get().deltaTime.count() * 1000.0f).c_str()
	);

    ImGui::End();
}

void StatsTool::OnSetInactive() { std::cout << "Deactive stats tool\n"; }
void StatsTool::OnSetActive() { std::cout << "Active stats tool\n"; }
} 
