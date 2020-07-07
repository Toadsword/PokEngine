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
// Date : 19.03.2020
//----------------------------------------------------------------------------------

#pragma once
#include <Utility/color.h>

namespace poke {
class ColorGradient {
public:
    struct ColorMark {
        bool operator==(const ColorMark& other) const
        {
            return color == other.color
                   && position == other.position;
        }

        bool operator!=(const ColorMark& other) const { return !(*this == other); }

        void SetFromJson(const json& markJson)
        {
			color.SetFromJson(markJson["Color"]);
			position = markJson["Position"];
        }

        json ToJson() const
        {
			json markJson;
			markJson["Color"] = color.ToJson();
			markJson["Position"] = position;

			return markJson;
        }

        Color color;
        //Goes from 0 to 1
        float position;
    };

    ColorGradient();
	~ColorGradient() = default;

    bool operator==(const ColorGradient& other) const;

    bool operator!=(const ColorGradient& other) const;

	Color GetColorAt(float position) const;
	void AddMark(float position, Color color);
	void RemoveMark(ColorMark mark);

    const std::vector<ColorMark>& GetMarks() const;

    void SetMarks(const std::vector<ColorMark>& marks);

    void SetFromJson(const json& gradientJson);

    json ToJson() const;

	inline const static int kMaxSize = 256;

private:
    void RefreshColors();

	Color ComputeColorAt(float position, Color color) const;

	std::vector<ColorMark> marks_; //TODO(@Nico) Change to std::array
	std::array<Color, kMaxSize> colors_;
};
} //namespace poke
