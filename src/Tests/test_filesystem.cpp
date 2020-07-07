#include <gtest/gtest.h>

#include <CoreEngine/engine.h>
#include <Editor/editor.h>
#include <Utility/file_system.h>

TEST(Filesystem, AbsolutePath)
{
	std::cout << poke::PokFileSystem::GetAbsoluteFullPath("pokEngine", poke::FileType::ENGINE_SETTING, poke::FolderType::SAVE_IN_ROM) << "\n";
	std::cout << poke::PokFileSystem::GetFullPath("pokEngine", poke::FileType::ENGINE_SETTING, poke::FolderType::SAVE_IN_ROM);
}