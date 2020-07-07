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
// Author : Robin Alves
// Co-Author :
// Date : 06.11.2019
//----------------------------------------------------------------------------------
#pragma once

#include <random>

#include <Math/vector.h>

namespace poke::math {
class Random {
public:
    static Random& Get();

    /**
    * \brief Give a random floating number between the minimum and the maximum but need two float
    */
    float RandomRange(float minimum, float maximum);

    /**
     * \brief Return a random integer number between the minimum and the maximum but need two int
     */
    int RandomRange(int minimum, int maximum);

    /**
     * \brief Return a random floating number between 0 and the maximum but need an int
     */
    float RandomRange(float maximum);

    /**
     * \brief Return a random integer number between 0 and the maximum but need an int
     */
    int RandomRange(int maximum);

    /**
     * \brief Set the seed for all the random functions
     */
    void SetSeedForRandom(int newSeed);

    static const unsigned int kNoSeed = 0;

private:
    std::random_device randomDevice_;

    unsigned int seed_ = 0;
    unsigned int iteration_ = 0;
};

const static float kPi = 3.141592653f;

const static float kRad2Deg = 180.0f / kPi;

const static float kDeg2Rad = kPi / 180.0f;

constexpr size_t log2(const size_t n)
{
	return ((n < 2) ? 0 : 1 + log2(n / 2));
}
} //namespace poke::math

namespace std {
/**
 * Combines a seed into a hash and modifies the seed by the new hash.
 * @param seed The seed.
 * @param v The value to hash.
 **/
template <typename T>
static void HashCombine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <>
struct hash<poke::math::Vec3> {
    size_t operator()(const poke::math::Vec3& vector) const
    {
        size_t seed = 0;
        HashCombine(seed, vector.x);
        HashCombine(seed, vector.y);
        HashCombine(seed, vector.z);
        return seed;
    }
};

template <>
struct hash<poke::math::Vec2> {
    size_t operator()(const poke::math::Vec2& vector) const
    {
        size_t seed = 0;
        HashCombine(seed, vector.x);
        HashCombine(seed, vector.y);
        return seed;
    }
};

template <>
struct hash<poke::math::Vec4> {
    size_t operator()(const poke::math::Vec4& vector) const
    {
        size_t seed = 0;
        HashCombine(seed, vector.x);
        HashCombine(seed, vector.y);
        HashCombine(seed, vector.z);
        HashCombine(seed, vector.w);
        return seed;
    }
};
} //namespace std
