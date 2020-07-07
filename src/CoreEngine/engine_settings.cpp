#include <CoreEngine/engine_settings.h>

#include <Utility/json_utility.h>

namespace poke {

EngineSetting EngineSetting::Create(const std::string& filePath) {
    auto file = LoadJson(filePath);

    EngineSetting engineSetting{};

    engineSetting.appType = static_cast<AppType>(file["AppType"]);

    engineSetting.frameRate = std::chrono::duration<
        double, std::milli>(file["frameRate"]);

    engineSetting.windowWidth = file["WindowWidth"];
    engineSetting.windowHeight = file["WindowHeight"];

    engineSetting.windowName = &file["WindowName"].get<std::string>()[0];

    return engineSetting;
}
} //namespace poke