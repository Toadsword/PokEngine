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
// Date : 18.03.2020
//-----------------------------------------------------------------------------
#pragma once

#include <vector>

#include <imgui.h>

struct ImGradientMark {
    bool operator==(const ImGradientMark& other) const
    {
        return
            color[0] == other.color[0] && 
            color[1] == other.color[1] && 
            color[2] == other.color[2] && 
            color[3] == other.color[3] && 
            position == other.position;
    }

    bool operator!=(const ImGradientMark& rhs) const
    {
        return !(*this == rhs);
    }

    float color[4];
    float position; //0 to 1
};

class ImGradient {
public:
    ImGradient();
    ~ImGradient();

    void GetColorAt(float position, float* color) const;
    void AddMark(float position, ImColor color);
    void RemoveMark(int index);
    void RefreshCache();
    std::vector<ImGradientMark>& GetMarks() { return marks_; }
    void SetMarks(const std::vector<ImGradientMark>& marks);
private:
    void ComputeColorAt(float position, float* color) const;
    std::vector<ImGradientMark> marks_;
    float cachedValues_[256 * 4];
};

namespace ImGui {
bool GradientButton(const char* name, ImGradient* gradient);

bool GradientEditor(
    ImGradient* gradient,
    int& draggingMarkIndex,
    int& selectedMarkIndex);
} //namespace ImGui
