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
// Date : 19.11.19
//----------------------------------------------------------------------------------

#pragma once
#include <string>

#include <Math/math.h>

namespace poke
{
class Color
{
public:
	Color() = default;

	/**
	 * \brief
	 * \param red
	 * \param green
	 * \param blue
	 * \param alpha
	 */
	explicit Color(float red, float green, float blue, float alpha = 1.0f);
	explicit Color(std::string hex, float alpha = 1.0f);

	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 0.0f;

	json ToJson() const
	{
		json vectorJson;
		vectorJson["r"] = r;
		vectorJson["g"] = g;
		vectorJson["b"] = b;
		vectorJson["a"] = a;
		return vectorJson;
	}

	void SetFromJson(const json& json)
	{
		r = json["r"];
		g = json["g"];
		b = json["b"];
		a = json["a"];
	}

	bool operator==(const Color &other) const;

	bool operator!=(const Color &other) const;

	Color operator*(const float rhs) const
	{
		return Color(r * rhs, g * rhs, b * rhs, a);
	}

	Color operator*(const Color other) const
	{
		return Color(r * other.r, g * other.g, b * other.b, a * other.a);
	}

	float &operator[](const uint32_t index);

	friend std::ostream &operator<<(std::ostream &stream, const Color &color);

	static const Color clear;
	static const Color black;
	static const Color grey;
	static const Color silver;
	static const Color white;
	static const Color maroon;
	static const Color red;
	static const Color olive;
	static const Color yellow;
	static const Color orange;
	static const Color green;
	static const Color lime;
	static const Color teal;
	static const Color aqua;
	static const Color navy;
	static const Color blue;
	static const Color purple;
	static const Color fuchsia;
};
} //namespace poke

namespace std
{
template<>
struct hash<poke::Color> {
	size_t operator()(const poke::Color &color) const noexcept {
		size_t seed = 0;
		HashCombine(seed, color.r);
		HashCombine(seed, color.g);
		HashCombine(seed, color.b);
		HashCombine(seed, color.a);
		return seed;
	}
};
}//std