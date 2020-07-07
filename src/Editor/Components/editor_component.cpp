#include <Editor/Components/editor_component.h>

namespace poke::ecs {

bool EditorComponent::operator==(const EditorComponent & other) const
{
	return name == other.name && 
		prefabName == other.prefabName;
}
bool EditorComponent::operator!=(const EditorComponent & other) const
{
	return !(*this == other);
}

const json EditorComponent::ToJson() const {
    json editorComponentJson = json::object();

	editorComponentJson["prefabName"] = prefabName;
    editorComponentJson["name"] = name;

    return editorComponentJson;
}

void EditorComponent::SetFromJson(const json& editorComponentJson) {

    if(CheckJsonExists(editorComponentJson, "prefabName")) {
        prefabName = editorComponentJson["prefabName"].get<std::string>();
    }
	if (CheckJsonExists(editorComponentJson, "name")) {
		name = editorComponentJson["name"].get<std::string>();
	}
}
}//namespace poke::ecs