//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
#pragma once
#include <fmod.h>
#include <Utility/log.h>

namespace poke {
namespace audio {
/**
 * \brief 
 * \param result 
 */
static void CheckFMOD(const FMOD_RESULT result)
{
    if (result != FMOD_OK) {
        switch (result) {
        case FMOD_ERR_BADCOMMAND:
            LogError(
                "Tried to call a function on a data type that does not allow this type of functionality (ie calling Sound::lock on a streaming sound).",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_CHANNEL_ALLOC:
            LogError("Error trying to allocate a channel.", LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_CHANNEL_STOLEN:
            LogError(
                "The specified channel has been reused to play another sound.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_DMA:
            LogError(
                "DMA Failure. See debug output for more information.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_DSP_CONNECTION:
            LogError(
                "DSP connection error. Connection possibly caused a cyclic dependency or connected dsps with incompatible buffer counts.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_DSP_DONTPROCESS:
            LogError(
                "DSP return code from a DSP process query callback. Tells mixer not to call the process callback and therefore not consume CPU. Use this to optimize the DSP graph.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_DSP_FORMAT:
            LogError(
                "DSP Format error. A DSP unit may have attempted to connect to this network with the wrong format, or a matrix may have been set with the wrong size if the target unit has a specified channel map.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_DSP_INUSE:
            LogError(
                "DSP is already in the mixer's DSP network. It must be removed before being reinserted or released.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_DSP_NOTFOUND:
            LogError(
                "DSP connection error. Couldn't find the DSP unit specified.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_DSP_RESERVED:
            LogError(
                "DSP operation error. Cannot perform operation on this DSP as it is reserved by the system.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_DSP_SILENCE:
            LogError(
                "DSP return code from a DSP process query callback. Tells mixer silence would be produced from read, so go idle and not consume CPU. Use this to optimize the DSP graph.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_DSP_TYPE:
            LogError(
                "DSP operation cannot be performed on a DSP of this type.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_FILE_BAD:
            LogError("Error loading file.", LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_FILE_COULDNOTSEEK:
            LogError(
                "Couldn't perform seek operation. This is a limitation of the medium (ie netstreams) or the file format.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_FILE_DISKEJECTED:
            LogError("Media was ejected while reading.", LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_FILE_EOF:
            LogError(
                "End of file unexpectedly reached while trying to read essential data (truncated?).",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_FILE_ENDOFDATA:
            LogError(
                "End of current chunk reached while trying to read data.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_FILE_NOTFOUND:
            LogError("File not found.", LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_FORMAT:
            LogError("Unsupported file or audio format.", LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_HEADER_MISMATCH:
            LogError(
                "There is a version mismatch between the FMOD header and either the FMOD Studio library or the FMOD Core library.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_HTTP:
            LogError(
                "A HTTP error occurred. This is a catch-all for HTTP errors not listed elsewhere.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_HTTP_ACCESS:
            LogError(
                "The specified resource requires authentication or is forbidden.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_HTTP_PROXY_AUTH:
            LogError(
                "Proxy authentication is required to access the specified resource.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_HTTP_SERVER_ERROR:
            LogError("A HTTP server error occurred.", LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_HTTP_TIMEOUT:
            LogError("The HTTP request timed out.", LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_INITIALIZATION:
            LogError(
                "FMOD was not initialized correctly to support this function.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_INITIALIZED:
            LogError(
                "Cannot call this command after System::init.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_INTERNAL:
            LogError(
                "An error occurred that wasn't supposed to. Contact support.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_INVALID_FLOAT:
            LogError(
                "Value passed in was a NaN, Inf or denormalized float.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_INVALID_HANDLE:
            LogError("An invalid object handle was used.", LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_INVALID_PARAM:
            LogError(
                "An invalid parameter was passed to this function.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_INVALID_POSITION:
            LogError(
                "An invalid seek position was passed to this function.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_INVALID_SPEAKER:
            LogError(
                "An invalid speaker was passed to this function based on the current speaker mode.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_INVALID_SYNCPOINT:
            LogError(
                "The syncpoint did not come from this sound handle.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_INVALID_THREAD:
            LogError(
                "Tried to call a function on a thread that is not supported.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_INVALID_VECTOR:
            LogError(
                "The vectors passed in are not unit length, or perpendicular.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_MAXAUDIBLE:
            LogError(
                "Reached maximum audible playback count for this sound's soundgroup.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_MEMORY:
            LogError("Not enough memory or resources.", LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_MEMORY_CANTPOINT:
            LogError(
                "Can't use FMOD_OPENMEMORY_POINT on non PCM source data, or non mp3/xma/adpcm data if FMOD_CREATECOMPRESSEDSAMPLE was used.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_NEEDS3D:
            LogError(
                "Tried to call a command on a 2d sound when the command was meant for 3d sound.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_NEEDSHARDWARE:
            LogError(
                "Tried to use a feature that requires hardware support.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_NET_CONNECT:
            LogError(
                "Couldn't connect to the specified host.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_NET_SOCKET_ERROR:
            LogError(
                "A socket error occurred. This is a catch-all for socket-related errors not listed elsewhere.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_NET_URL:
            LogError(
                "The specified URL couldn't be resolved.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_NET_WOULD_BLOCK:
            LogError(
                "Operation on a non-blocking socket could not complete immediately.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_NOTREADY:
            LogError(
                "Operation could not be performed because specified sound/DSP connection is not ready.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_OUTPUT_ALLOCATED:
            LogError(
                "Error initializing output device, but more specifically, the output device is already in use and cannot be reused.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_OUTPUT_CREATEBUFFER:
            LogError(
                "Error creating hardware sound buffer.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_OUTPUT_DRIVERCALL:
            LogError(
                "A call to a standard soundcard driver failed, which could possibly mean a bug in the driver or resources were missing or exhausted.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_OUTPUT_FORMAT:
            LogError(
                "Soundcard does not support the specified format.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_OUTPUT_INIT:
            LogError("Error initializing output device.", LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_OUTPUT_NODRIVERS:
            LogError(
                "The output device has no drivers installed. If pre-init, FMOD_OUTPUT_NOSOUND is selected as the output mode. If post-init, the function just fails.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_PLUGIN:
            LogError(
                "An unspecified error has been returned from a plugin.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_PLUGIN_MISSING:
            LogError(
                "A requested output, dsp unit type or codec was not available.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_PLUGIN_RESOURCE:
            LogError(
                "A resource that the plugin requires cannot be found. (ie the DLS file for MIDI playback)",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_PLUGIN_VERSION:
            LogError(
                "A plugin was built with an unsupported SDK version.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_RECORD:
            LogError(
                "An error occurred trying to initialize the recording device.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_REVERB_CHANNELGROUP:
            LogError(
                "Reverb properties cannot be set on this channel because a parent channelgroup owns the reverb connection.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_REVERB_INSTANCE:
            LogError(
                "Specified instance in FMOD_REVERB_PROPERTIES couldn't be set. Most likely because it is an invalid instance number or the reverb doesn't exist.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_SUBSOUNDS:
            LogError(
                "The error occurred because the sound referenced contains subsounds when it shouldn't have, or it doesn't contain subsounds when it should have. The operation may also not be able to be performed on a parent sound.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_SUBSOUND_ALLOCATED:
            LogError(
                "This subsound is already being used by another sound, you cannot have more than one parent to a sound. Null out the other parent's entry first.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_SUBSOUND_CANTMOVE:
            LogError(
                "Shared subsounds cannot be replaced or moved from their parent stream, such as when the parent stream is an FSB file.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_TAGNOTFOUND:
            LogError(
                "The specified tag could not be found or there are no tags.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_TOOMANYCHANNELS:
            LogError(
                "The sound created exceeds the allowable input channel count. This can be increased using the 'maxinputchannels' parameter in System::setSoftwareFormat.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_TRUNCATED:
            LogError(
                "The retrieved string is too long to fit in the supplied buffer and has been truncated.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_UNIMPLEMENTED:
            LogError(
                "Something in FMOD hasn't been implemented when it should be! contact support!",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_UNINITIALIZED:
            LogError(
                "This command failed because System::init or System::setDriver was not called.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_UNSUPPORTED:
            LogError(
                "A command issued was not supported by this object. Possibly a plugin without certain callbacks specified.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_VERSION:
            LogError(
                "The version number of this file format is not supported.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_EVENT_ALREADY_LOADED:
            LogError(
                "The specified bank has already been loaded.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_EVENT_LIVEUPDATE_BUSY:
            LogError(
                "The live update connection failed due to the game already being connected.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_EVENT_LIVEUPDATE_MISMATCH:
            LogError(
                "The live update connection failed due to the game data being out of sync with the tool.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_EVENT_LIVEUPDATE_TIMEOUT:
            LogError(
                "The live update connection timed out.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_EVENT_NOTFOUND:
            LogError(
                "The requested event, parameter, bus or vca could not be found.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_STUDIO_UNINITIALIZED:
            LogError(
                "The Studio::System object is not yet initialized.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_STUDIO_NOT_LOADED:
            LogError(
                "The specified resource is not loaded, so it can't be unloaded.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_INVALID_STRING:
            LogError(
                "An invalid string was passed to this function.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_ALREADY_LOCKED:
            LogError(
                "The specified resource is already locked.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_NOT_LOCKED:
            LogError(
                "The specified resource is not locked, so it can't be unlocked.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_RECORD_DISCONNECTED:
            LogError(
                "The specified recording driver has been disconnected.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_ERR_TOOMANYSAMPLES:
            LogError(
                "The length provided exceeds the allowable limit.",
                LogType::AUDIO_LOG);
            break;
        case FMOD_RESULT_FORCEINT:
            LogError("", LogType::AUDIO_LOG);
            break;
        default: ;
        }
    }
}
} //namespace audio
} //namespace poke
