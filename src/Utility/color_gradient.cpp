#include <Utility/color_gradient.h>

#include <algorithm>

namespace poke {

ColorGradient::ColorGradient()
{
    AddMark(0, Color::white);
    AddMark(1, Color::white);
}

bool ColorGradient::operator==(const ColorGradient& other) const
{
    return marks_ == other.marks_
           && colors_ == other.colors_;
}

bool ColorGradient::operator!=(const ColorGradient& other) const { return !(*this == other); }

Color ColorGradient::GetColorAt(const float position) const
{
    float pos = position;
    if (pos < 0.0f)
        pos = 0.0f;
    if (pos > 1.0f)
        pos = 1.0f;

    return colors_[pos * (kMaxSize - 1)];
}

void ColorGradient::AddMark(const float position, const Color color)
{
    float pos = position;
    if (pos < 0.0f)
        pos = 0.0f;
    if (pos > 1.0f)
        pos = 1.0f;

    ColorMark newMark;
    newMark.position = pos;
    newMark.color = color;

    marks_.push_back(newMark);

    RefreshColors();
}

void ColorGradient::RemoveMark(const ColorMark mark)
{
    const auto it = std::find(marks_.begin(), marks_.end(), mark);
    marks_.erase(it);
    RefreshColors();
}

const std::vector<ColorGradient::ColorMark>& ColorGradient::GetMarks() const { return marks_; }

void ColorGradient::SetMarks(const std::vector<ColorMark>& marks)
{
    marks_ = marks;
    RefreshColors();
}

void ColorGradient::SetFromJson(const json& gradientJson)
{
    marks_.resize(gradientJson["ColorGradient"]["marks"].size());
    for (size_t i = 0; i < marks_.size(); i++) {
        marks_[i].SetFromJson(gradientJson["ColorGradient"]["marks"][i]);
    }

    RefreshColors();
}

json ColorGradient::ToJson() const
{
    json gradientJson;
    for (size_t i = 0; i < marks_.size(); i++) {
        gradientJson["ColorGradient"]["marks"][i] = marks_[i].ToJson();
    }

    return gradientJson;
}

void ColorGradient::RefreshColors()
{
    std::sort(
        marks_.begin(),
        marks_.end(),
        [](const ColorMark a, const ColorMark b) { return a.position < b.position; });

    for (int i = 0; i < kMaxSize; ++i) {
        colors_[i] = ComputeColorAt(static_cast<float>(i) / (kMaxSize - 1.0f), colors_[i]);
    }
}

Color ColorGradient::ComputeColorAt(const float position, const Color color) const
{
    Color newColor = color;

    float pos = position;

    if (pos < 0.0f)
        pos = 0.0f;
    if (pos > 1.0f)
        pos = 1.0f;

    ColorMark lower = marks_[0];
    ColorMark upper = marks_[marks_.size() - 1];

    for (const ColorMark mark : marks_) {
        if (mark.position < pos) { if (lower.position < mark.position) { lower = mark; } }

        if (mark.position >= pos) { if (upper.position > mark.position) { upper = mark; } }
    }

    if (upper == lower) { newColor = upper.color; } else {
        const float distance = upper.position - lower.position;
        const float delta = (pos - lower.position) / distance;

        //lerp
        newColor[0] = (1.0f - delta) * lower.color[0] + delta * upper.color[0];
        newColor[1] = (1.0f - delta) * lower.color[1] + delta * upper.color[1];
        newColor[2] = (1.0f - delta) * lower.color[2] + delta * upper.color[2];
        newColor[3] = (1.0f - delta) * lower.color[3] + delta * upper.color[3];
    }

    return newColor;
}
} //namespace poke
