#include <Editor/editor_utility.h>

#include <imgui.h>

#include <Utility/color.h>

namespace poke {
namespace editor {
Color ImGuiToPokColor(ImVec4 imGuiColor) {
    return Color(imGuiColor.x, imGuiColor.y, imGuiColor.z, imGuiColor.w);
}

ImVec4 PokToImGuiColor(Color pokColor) {
    return ImVec4(pokColor.r, pokColor.g, pokColor.b, pokColor.a);
}
} //namespace editor
} //namespace poke
