//----------------------------------------------------------------------------------
// Copyright (c) 2019-2020, POK Family. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of POK Family nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author : Nicolas Schneider
// Co-Author :
// Date : 18.02.20
//----------------------------------------------------------------------------------
#pragma once

#include <string>
#include <map>
#include <vector>

#include <fmod.hpp>
#include <fmod_studio.hpp>

namespace poke {
namespace audio {
using Clip = FMOD::Sound*;

inline static const int maxChannel = 32;
inline static const float kMaxVolume = 100;
/**
 * \brief This class is the implementation of FMOD. It's used to avoid having cyclic errors. And it will later be used to encapsulate FMOD.
 */
struct Implementation {

	using SoundMap = std::map<std::string, FMOD::Sound*>;
	using ChannelMap = std::map<int, FMOD::Channel*>;
	using BankMap = std::map<std::string, FMOD::Studio::Bank*>;

    Implementation();
    ~Implementation();

	void Init();

    void Update();

	void Destroy();

	FMOD::Studio::System* studioSystem;
	FMOD::System* system;

	int nextChannelId{};

	BankMap banks;
	ChannelMap channels;
};
} //namespace audio
} //namespace poke
