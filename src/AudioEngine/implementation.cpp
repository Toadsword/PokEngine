#include <AudioEngine/implementation.h>

#include <Utility/log.h>
#include <AudioEngine/fmod_error_handler.h>

namespace poke {
namespace audio {

Implementation::Implementation()
    : studioSystem(nullptr),
      system(nullptr) { }

Implementation::~Implementation()
{

}

void Implementation::Init()
{
    //Create main StudioSystem
	CheckFMOD(FMOD::Studio::System::create(&studioSystem));

	//Get low level core system
	CheckFMOD(studioSystem->getCoreSystem(&system));
	CheckFMOD(system->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0));

    //Init StudioSystems
	CheckFMOD(
		studioSystem->initialize(
			maxChannel,
			FMOD_STUDIO_INIT_NORMAL,
			FMOD_INIT_NORMAL,
			nullptr));

	system->set3DSettings(1, 1, 1);
}

void Implementation::Update()
{
    //Check all channel, if one is not playing then delete it
    std::vector<ChannelMap::iterator> stoppedChannels;
	stoppedChannels.reserve(channels.size());
    for (auto it = channels.begin(); it != channels.end(); ++it) {
        auto isPlaying = false;
        it->second->isPlaying(&isPlaying);
        if (!isPlaying) { stoppedChannels.push_back(it); }
    }
	stoppedChannels.shrink_to_fit();

    for (auto& it : stoppedChannels) { channels.erase(it); }
    //Update studio system
	CheckFMOD(studioSystem->update());
}

void Implementation::Destroy()
{
	CheckFMOD(studioSystem->unloadAll());
	CheckFMOD(studioSystem->release());


    for (auto bank : banks) {
		CheckFMOD(bank.second->unload());
    }
}
} //namespace audio
} //namespace poke
