// StressTestFileSystemSwitch.cpp : Defines the entry point to the NintendoSDK application.
//
#include <Tests/Switch/stressTestFileSystem.h>

#include <iostream>
#include <fstream>

#include <Utility/file_system.h>
#include <Utility/chronometer.h>

namespace poke::test {
void ReadAllFiles() {
	std::string str = "";

	poke::Chronometer chrono;
	double time = chrono.GetTimeMilli();
	str = "";

	// ----------------- one scene file --------------- //
	chrono.Restart();
	str = "";
	for (int i = 1; i <= 3; i++)
		str += poke::PokFileSystem::ReadFile("scenes/outputSceneFile.pokscene", poke::FileType::CUSTOM, poke::FolderType::ROM);
	time = chrono.GetTimeMilli();
	std::cout << "Time to read three small scene file : " << time << "\n";

	// ----------------- one big scene file --------------- //
	chrono.Restart();
	str = poke::PokFileSystem::ReadFile("scenes/outputBigSceneFile.pokscene", poke::FileType::CUSTOM, poke::FolderType::ROM);
	time = chrono.GetTimeMilli();
	std::cout << "Time to read one big scene file : " << time << "\n";

	// ----------------- one big opti file --------------- //
	chrono.Restart();
	str = poke::PokFileSystem::ReadFile("scenes/outputBigSceneFileOpti.pokscene", poke::FileType::CUSTOM, poke::FolderType::ROM);
	time = chrono.GetTimeMilli();
	std::cout << "Time to read one big opti file : " << time << "\n";

	// Calculating
	std::vector<std::string> strs(100, std::string(""));
	chrono.Restart();
	// ----------------- hundred empty files --------------- //
	for (int i = 0; i < 100; i++) {
		strs[i] = poke::PokFileSystem::ReadFile("emptyFile.txt", poke::FileType::CUSTOM, poke::FolderType::ROM);
	}
	time = chrono.GetTimeMilli();
	std::cout << "Time to read 100 empty file : " << time << "\n";

	// one shader
	str = "";
	chrono.Restart();
	str = poke::PokFileSystem::ReadFile("default", poke::FileType::SHADER, poke::FolderType::ROM);

	time = chrono.GetTimeMilli();
	std::cout << "Time to read one shader file : " << time << "\n";

	// hundred of shaders
	strs = std::vector<std::string>(100, std::string(""));
	chrono.Restart();
	for (int i = 0; i < 100; i++) {
		strs[i] = poke::PokFileSystem::ReadFile("default", poke::FileType::SHADER, poke::FolderType::ROM);
	}
	time = chrono.GetTimeMilli();
	std::cout << "Time to read 100 shader file : " << time << "\n";

	// one scene
	str = "";
	chrono.Restart();
	str = poke::PokFileSystem::ReadFile("scenes/outputSceneFile.pokscene", poke::FileType::CUSTOM, poke::FolderType::ROM);

	time = chrono.GetTimeMilli();
	std::cout << "Time to read one scene file : " << time << "\n";

	// hundred of scenes
	strs = std::vector<std::string>(100, std::string(""));
	chrono.Restart();
	for (int i = 0; i < 100; i++) {
		strs[i] = poke::PokFileSystem::ReadFile("scenes/outputSceneFile.pokscene", poke::FileType::CUSTOM, poke::FolderType::ROM);
	}
	time = chrono.GetTimeMilli();
	std::cout << "Time to read 100 scene file : " << time << "\n";

}

int StressTestFileSystem(){
	std::cout << "FileSystem stress test\n";
	std::cout << "-------------- ALL FILE NORMAL ------------\n";

	ReadAllFiles();

    //Without reloading Mount
#ifdef NN_NINTENDO_SDK
	poke::PokFileSystem::MountFileSystem(poke::FolderType::ROM);
#endif
	std::cout << "-------------- ALL FILE WITH PRE-MOUNT ------------\n";
	
	ReadAllFiles();

#ifdef NN_NINTENDO_SDK
	poke::PokFileSystem::UnmountFileSystem(poke::FolderType::ROM);
#endif

	return 1;

}
}