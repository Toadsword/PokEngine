#include <Editor/imgui_gradient.h>

#include <algorithm>

#include <imgui_internal.h>

static const float kGradientBarWidgetHeight = 20;
static const float kGradientBarEditorHeight = 40;
static const float kGradientMarkDeleteDiffy = 40;

ImGradient::ImGradient()
{
    AddMark(0.0f, ImColor(0.0f, 0.0f, 0.0f));
    AddMark(1.0f, ImColor(1.0f, 1.0f, 1.0f));
}

ImGradient::~ImGradient() {}

void ImGradient::AddMark(float position, const ImColor color)
{
    position = ImClamp(position, 0.0f, 1.0f);
    ImGradientMark newMark{};
    newMark.position = position;
    newMark.color[0] = color.Value.x;
    newMark.color[1] = color.Value.y;
    newMark.color[2] = color.Value.z;
    newMark.color[3] = color.Value.w;

    marks_.push_back(newMark);

    RefreshCache();
}

void ImGradient::RemoveMark(const int index)
{
    marks_.erase(marks_.begin() + index);
    RefreshCache();
}

void ImGradient::GetColorAt(float position, float* color) const
{
    position = ImClamp(position, 0.0f, 1.0f);
    int cachePos = position * 255;
    cachePos *= 4;
    color[0] = cachedValues_[cachePos + 0];
    color[1] = cachedValues_[cachePos + 1];
    color[2] = cachedValues_[cachePos + 2];
    color[3] = cachedValues_[cachePos + 3];
}

void ImGradient::SetMarks(const std::vector<ImGradientMark>& marks)
{
    marks_.clear();

    for (auto mark : marks) {
        AddMark(mark.position, ImColor(mark.color[0], mark.color[1], mark.color[2], mark.color[3]));
    }
}

void ImGradient::ComputeColorAt(float position, float* color) const
{
    position = ImClamp(position, 0.0f, 1.0f);

    ImGradientMark lower = marks_[0];
    ImGradientMark upper = marks_[0];

    for (ImGradientMark mark : marks_) {
        if (mark.position < position) {
            if (lower.position < mark.position) { lower = mark; }
        }

        if (mark.position >= position) {
            if (upper.position > mark.position) { upper = mark; }
        }
    }

    if (upper == lower) {
        color[0] = upper.color[0];
        color[1] = upper.color[1];
        color[2] = upper.color[2];
        color[3] = upper.color[3];
    } else {
        const float distance = upper.position - lower.position;
        const float delta = (position - lower.position) / distance;

        //lerp
        color[0] = (1.0f - delta) * lower.color[0] + delta * upper.color[0];
        color[1] = (1.0f - delta) * lower.color[1] + delta * upper.color[1];
        color[2] = (1.0f - delta) * lower.color[2] + delta * upper.color[2];
        color[3] = (1.0f - delta) * lower.color[3] + delta * upper.color[3];
    }
}

void ImGradient::RefreshCache()
{
	std::sort(marks_.begin(), marks_.end(), [](const ImGradientMark a, const ImGradientMark b) { return a.position < b.position; });

    for (int i = 0; i < 256; ++i) { ComputeColorAt(i / 255.0f, &cachedValues_[i * 4]); }
}


namespace ImGui {
static void DrawGradientBar(
    ImGradient* gradient,
    struct ImVec2 const& barPos,
    const float maxWidth,
    const float height)
{
    ImVec4 colorA = {1, 1, 1, 1};
    ImVec4 colorB = {1, 1, 1, 1};
    float prevX = barPos.x;
    const float barBottom = barPos.y + height;
    ImGradientMark* prevMark = nullptr;
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    drawList->AddRectFilled(
        ImVec2(barPos.x - 2, barPos.y - 2),
        ImVec2(barPos.x + maxWidth + 2, barBottom + 2),
        IM_COL32(100, 100, 100, 255));

    if (gradient->GetMarks().empty()) {
        drawList->AddRectFilled(
            ImVec2(barPos.x, barPos.y),
            ImVec2(barPos.x + maxWidth, barBottom),
            IM_COL32(255, 255, 255, 255));
    }

    ImU32 colorAU32 = 0;
    ImU32 colorBU32 = 0;

    for (auto& i : gradient->GetMarks()) {
        const float from = prevX;
		auto mark = i;
        const float to = prevX = barPos.x + mark.position * maxWidth;

        if (prevMark == nullptr) {
            colorA.x = mark.color[0];
            colorA.y = mark.color[1];
            colorA.z = mark.color[2];
            colorA.w = mark.color[3];
        } else {
            colorA.x = prevMark->color[0];
            colorA.y = prevMark->color[1];
            colorA.z = prevMark->color[2];
            colorA.w = prevMark->color[3];
        }

        colorB.x = mark.color[0];
        colorB.y = mark.color[1];
        colorB.z = mark.color[2];
        colorB.w = mark.color[3];

        colorAU32 = ImGui::ColorConvertFloat4ToU32(colorA);
        colorBU32 = ImGui::ColorConvertFloat4ToU32(colorB);

        if (mark.position > 0.0) {
            drawList->AddRectFilledMultiColor(
                ImVec2(from, barPos.y),
                ImVec2(to, barBottom),
                colorAU32,
                colorBU32,
                colorBU32,
                colorAU32);
        }

        prevMark = &i;
    }

    if (prevMark && prevMark->position < 1.0) {
        drawList->AddRectFilledMultiColor(
            ImVec2(prevX, barPos.y),
            ImVec2(barPos.x + maxWidth, barBottom),
            colorBU32,
            colorBU32,
            colorBU32,
            colorBU32);
    }

    ImGui::SetCursorScreenPos(ImVec2(barPos.x, barPos.y + height + 10.0f));
}

static void DrawGradientMarks(
    ImGradient* gradient,
	int& draggingMarkIndex,
	int& selectedMarkIndex,
    struct ImVec2 const& barPos,
    const float maxWidth,
    const float height)
{
    ImVec4 colorA = {1, 1, 1, 1};
    ImVec4 colorB = {1, 1, 1, 1};
    float barBottom = barPos.y + height;
    int prevMarkIndex = -1;
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImU32 colorAU32 = 0;
    ImU32 colorBU32 = 0;

    for (auto i = 0; i < gradient->GetMarks().size(); i++) {

        if (selectedMarkIndex == -1) {
			selectedMarkIndex = i;
        }

		auto mark = gradient->GetMarks()[i];

        float to = barPos.x + mark.position * maxWidth;

        if (prevMarkIndex == -1) {
            colorA.x = mark.color[0];
            colorA.y = mark.color[1];
            colorA.z = mark.color[2];
            colorA.w = mark.color[3];
        } else {
            colorA.x = gradient->GetMarks()[prevMarkIndex].color[0];
            colorA.y = gradient->GetMarks()[prevMarkIndex].color[1];
            colorA.z = gradient->GetMarks()[prevMarkIndex].color[2];
            colorA.w = gradient->GetMarks()[prevMarkIndex].color[3];
        }

        colorB.x = mark.color[0];
        colorB.y = mark.color[1];
        colorB.z = mark.color[2];
        colorB.w = mark.color[3];

        colorAU32 = ImGui::ColorConvertFloat4ToU32(colorA);
        colorBU32 = ImGui::ColorConvertFloat4ToU32(colorB);

        drawList->AddTriangleFilled(
            ImVec2(to, barPos.y + (height - 6)),
            ImVec2(to - 6, barBottom),
            ImVec2(to + 6, barBottom),
            IM_COL32(100, 100, 100, 255));

        drawList->AddRectFilled(
            ImVec2(to - 6, barBottom),
            ImVec2(to + 6, barPos.y + (height + 12)),
            IM_COL32(100, 100, 100, 255),
            1.0f,
            1.0f);

        drawList->AddRectFilled(
            ImVec2(to - 5, barPos.y + (height + 1)),
            ImVec2(to + 5, barPos.y + (height + 11)),
            IM_COL32(0, 0, 0, 255),
            1.0f,
            1.0f);

        if (selectedMarkIndex == i) {
            drawList->AddTriangleFilled(
                ImVec2(to, barPos.y + (height - 3)),
                ImVec2(to - 4, barBottom + 1),
                ImVec2(to + 4, barBottom + 1),
                IM_COL32(0, 255, 0, 255));

            drawList->AddRect(
                ImVec2(to - 5, barPos.y + (height + 1)),
                ImVec2(to + 5, barPos.y + (height + 11)),
                IM_COL32(0, 255, 0, 255),
                1.0f,
                1.0f);
        }

        drawList->AddRectFilledMultiColor(
            ImVec2(to - 3, barPos.y + (height + 3)),
            ImVec2(to + 3, barPos.y + (height + 9)),
            colorBU32,
            colorBU32,
            colorBU32,
            colorBU32);

        ImGui::SetCursorScreenPos(ImVec2(to - 6, barBottom));
        ImGui::InvisibleButton("mark", ImVec2(12, 12));

        if (ImGui::IsItemHovered()) {
            if (ImGui::IsMouseClicked(0)) {
                selectedMarkIndex = i;
                draggingMarkIndex = i;
            }
        }


        prevMarkIndex = i;
    }

    ImGui::SetCursorScreenPos(ImVec2(barPos.x, barPos.y + height + 20.0f));
}

bool GradientButton(const char* name, ImGradient* gradient)
{
    if (!gradient)
        return false;

    const ImVec2 widgetPos = ImGui::GetCursorScreenPos();
    // ImDrawList* draw_list = ImGui::GetWindowDrawList();

    
    const float maxWidth = ImGui::CalcItemWidth();
    const bool clicked = ImGui::InvisibleButton(
        "gradient_bar",
        ImVec2(maxWidth, kGradientBarWidgetHeight));
    DrawGradientBar(gradient, widgetPos, maxWidth, kGradientBarWidgetHeight);
	ImGui::SameLine();
	ImGui::Text(name);

    return clicked;
}

bool GradientEditor(
    ImGradient* gradient,
	int& draggingMarkIndex,
	int& selectedMarkIndex)
{
    if (!gradient)
        return false;

    bool modified = false;

    ImVec2 barPos = ImGui::GetCursorScreenPos();
    barPos.x += 10;
    const float maxWidth = ImGui::GetContentRegionAvailWidth() - 20;
    const float barBottom = barPos.y + kGradientBarEditorHeight;

    ImGui::InvisibleButton("gradient_editor_bar", ImVec2(maxWidth, kGradientBarEditorHeight));

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
        const float pos = (ImGui::GetIO().MousePos.x - barPos.x) / maxWidth;

        float newMarkCol[4];
        gradient->GetColorAt(pos, newMarkCol);


        gradient->AddMark(pos, ImColor(newMarkCol[0], newMarkCol[1], newMarkCol[2], newMarkCol[3]));
    }

    DrawGradientBar(gradient, barPos, maxWidth, kGradientBarEditorHeight);
    DrawGradientMarks(
        gradient,
        draggingMarkIndex,
        selectedMarkIndex,
        barPos,
        maxWidth,
        kGradientBarEditorHeight);

    if (!ImGui::IsMouseDown(0) && draggingMarkIndex != -1) { draggingMarkIndex = -1; }

    if (ImGui::IsMouseDragging(0) && draggingMarkIndex != -1) {
        const float increment = ImGui::GetIO().MouseDelta.x / maxWidth;
        const bool insideZone = (ImGui::GetIO().MousePos.x > barPos.x) &&
                                (ImGui::GetIO().MousePos.x < barPos.x + maxWidth);

        if (increment != 0.0f && insideZone) {
            gradient->GetMarks()[draggingMarkIndex].position += increment;
			gradient->GetMarks()[draggingMarkIndex].position = ImClamp(gradient->GetMarks()[draggingMarkIndex].position, 0.0f, 1.0f);
            gradient->RefreshCache();
            modified = true;
        }

        const float diffY = ImGui::GetIO().MousePos.y - barBottom;

        if (diffY >= kGradientMarkDeleteDiffy) {
            gradient->RemoveMark(draggingMarkIndex);
			draggingMarkIndex = -1;
            selectedMarkIndex = -1;
            modified = true;
        }
    }

    if (selectedMarkIndex == -1 && !gradient->GetMarks().empty()) {
		selectedMarkIndex = 0;
    }

    if (selectedMarkIndex != -1) {
		auto color = gradient->GetMarks()[selectedMarkIndex].color;
        const bool colorModified = ImGui::ColorPicker4("color", color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_HDR);
		gradient->GetMarks()[selectedMarkIndex].color[0] = color[0];
		gradient->GetMarks()[selectedMarkIndex].color[1] = color[1];
		gradient->GetMarks()[selectedMarkIndex].color[2] = color[2];
		gradient->GetMarks()[selectedMarkIndex].color[3] = color[3];


        if (selectedMarkIndex != -1 && colorModified) {
            modified = true;
            gradient->RefreshCache();
        }
    }

    return modified;
}
};
