#include <Ecs/Entities/tag_manager.h>

#include <CoreEngine/engine.h>

namespace poke {
namespace ecs {
TagManager::TagManager(Engine& engine)
    : engine_(engine)
{
	engine.AddObserver(observer::MainLoopSubject::ENGINE_INIT, [this]() {OnEngineInit(); });
}

TagManager::~TagManager()
{
	const std::string tagFileName = engine_.GetEngineSettings().GetTagFileName();

	if (tagFileName != "") {
		const auto jsonTag = ToJson();
		poke::PokFileSystem::WriteFile(tagFileName, jsonTag, FileType::CUSTOM, poke::FolderType::SAVE_IN_ROM);
	}
}

void TagManager::SetFromJson(const json& tagsJson)
{
	tagNames_.clear();
	tagNames_.resize(0);
    for (const auto& tagJson : tagsJson) {
        tagNames_.push_back(tagJson.get<std::string>());
    }
}

json TagManager::ToJson()
{
    json tagsJson;

    for (size_t i = 0; i < tagNames_.size(); i++) {
        tagsJson[i] = tagNames_[i];
    }

    return tagsJson;
}

EntityTag TagManager::GetTag(const std::string& tagName)
{
    for (auto i = 0; i < tagNames_.size(); i++) {
        if (tagNames_[i] == tagName) { return i; }
    }

    return 0;
}

EntityTag TagManager::GetTagMaxValue() const { return kMaxFlagValue; }

const char* TagManager::GetTagName(const EntityTag tag) const
{
    return tagNames_[tag].c_str();
}

const std::vector<std::string>& TagManager::GetAllTagNames() const
{
    return tagNames_;
}

void TagManager::OnEngineInit()
{
    const std::string tagFileName = engine_.GetEngineSettings().GetTagFileName();

    if(tagFileName != "") {
        const auto jsonTag = LoadJson(tagFileName, FileType::CUSTOM);
		SetFromJson(jsonTag);
    }
}
} //namespace ecs
} //namespace poke
