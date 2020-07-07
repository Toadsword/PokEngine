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
// Date : 10.12.19
//----------------------------------------------------------------------------------
#pragma once
#include <string>
#include <Editor/editor_utility.h>

namespace poke {
namespace editor {

class Tool {
public:
    Tool(bool defaultActive = false);
    virtual ~Tool() = default;

    /**
     * \brief Update the active state of the tool and fire an event if the state change.
     */
    void UpdateActiveState();

    /**
	 * \brief Called when the tool needs to be drawn 
	 */
    virtual void OnDrawImgui() = 0; //Must be overridden 

    /**
	 * \brief Called when the tool is deactivate
	 */
    virtual void OnSetInactive() = 0;

    /**
	 * \brief Called when the tool is activated. It's called at the first frame if the tool start in an active state
	 */
    virtual void OnSetActive() = 0;

    void SetActive(bool b);

    bool IsActive() const;

    const char* GetName() const;

	ToolFlags GetFlag() const;

protected:
    bool isActive_ = true;
	ToolFlags toolFlag_ = TOOL_FLAG_NONE;

    std::string name_ = "Unnamed Tool";

private:
    enum class State : uint8_t {
        IDLE,
        ACTIVE,
        INACTIVE
    };

    State state_ = State::IDLE;
};
} //namespace editor
} //namespace poke
