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
// Author : Simon Canas
// Co-Author : Robin Alves
// Date : 17.01.2020
//----------------------------------------------------------------------------------
#pragma once

#include <Math/vector.h>
#include <Math/matrix.h>

namespace poke::math {

class Quaternion {
public:
    //-----------------------------------------------------------------------------
    // Constructors
    //-----------------------------------------------------------------------------
    Quaternion();
    Quaternion(float value);
    Quaternion(Vec3 v, float w);
    Quaternion(float x, float y, float z, float w);

    //-----------------------------------------------------------------------------
    // Formulas
    //-----------------------------------------------------------------------------
    void Normalize();
    static Quaternion Normalize(Quaternion q);
    float Magnitude() const;
    static float Dot(Quaternion lhs, Quaternion rhs);
    Matrix3 ToRotationMatrix3() const;
    Matrix4 ToRotationMatrix4() const;
    Quaternion GetConjugate() const;
    Quaternion GetInverse() const;

    /**
     * \brief Rotates a Quaternion around a specified axis
     * \param rhs the normalized vector that defines the axis
     * \param rot the rotation needed (in rads)
     * \return The rotated Quaternion
     */
    static Quaternion RotateAroundAxis(Vec3 rhs, float rot);
    Vec3 GetEulerAngles() const;

	static Quaternion FromEulerRad(const Vec3 euler);

    static Quaternion FromToRotation(Vec3 fromDirection, Vec3 toDirection);

    static Quaternion RotateTowards(
        Quaternion from,
        Quaternion to,
        float maxDegreesDelta);

    static float Angle(Quaternion a, Quaternion b);

	static Quaternion LookRotation(Vec3 forward, Vec3 up);

    static Quaternion LookRotation(Vec3 forward);

    //-----------------------------------------------------------------------------
    // Operators
    //-----------------------------------------------------------------------------
    Quaternion operator+(const Quaternion& rhs) const;
    Quaternion operator-(const Quaternion& rhs) const;
    Quaternion operator*(const Quaternion& rhs) const;
    Quaternion operator/(const Quaternion& rhs) const;

    bool operator==(const Quaternion& rhs) const;
    bool operator!=(const Quaternion& rhs) const;

    Quaternion operator+(float rhs) const;
    void operator+=(float rhs);
    Quaternion operator-(float rhs) const;
    void operator-=(float rhs);
    Quaternion operator*(float rhs) const;
    void operator*=(float rhs);
    Quaternion operator/(float rhs) const;
    void operator/=(float rhs);


private:
    float LengthSquared() const;

    static Vec3 ToEulerRad(const Quaternion rotation);

    static Vec3 NormalizeAngles(Vec3 angles);

    static float NormalizeAngle(float angle);

    static Quaternion SlerpUnclamped(Quaternion a, Quaternion b, float t);

	float x, y, z, w;
    //The actual quaternion represented by a 4 dimensional vector
};
}
