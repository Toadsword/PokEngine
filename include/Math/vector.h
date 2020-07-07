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
// Date : 06.11.2019
//----------------------------------------------------------------------------------

#pragma once

#include <array>
#include <cmath>
#include <string>

#include <CoreEngine/cassert.h>
#include <Utility/json_utility.h>

namespace poke::math {
template <typename T>
class Vector2 {
public:
    Vector2()
        : x(static_cast<T>(0)),
          y(static_cast<T>(0)) {}

    Vector2(const T value)
        : x(value),
          y(value) {}

    Vector2(const T x, const T y)
        : x(x),
          y(y) {}

    template <typename U>
    Vector2(const Vector2<U> v)
        : x(static_cast<T>(v.x)),
          y(static_cast<T>(v.y)) {}

    T& operator[](const int index)
    {
        if (index == 0)
            return x;
        else if (index == 1)
            return y;

        cassert(false, "Vector2 out of range!");
    }

    T operator[](const int index) const
    {
        if (index == 0)
            return x;
        else if (index == 1)
            return y;

        cassert(false, "Vector2 out of range!");
    }

    bool operator==(const Vector2<T> v) const { return x == v.x && y == v.y; }
    bool operator!=(const Vector2<T> v) const { return !(*this == v); }

    Vector2<T> operator+(const Vector2<T> v) const
    {
        return Vector2<T>(
            x + v.x,
            y + v.y
        );
    }

    void operator+=(const Vector2<T> v)
    {
        x += v.x;
        y += v.y;
    }

    Vector2<T> operator-(const Vector2<T> v) const
    {
        return Vector2<T>(
            x - v.x,
            y - v.y
        );
    }

    void operator-=(const Vector2<T> v)
    {
        x -= v.x;
        y -= v.y;
    }

    Vector2<T> operator*(const T value) const
    {
        return Vector2<T>(
            x * value,
            y * value
        );
    }

    void operator*=(const T value)
    {
        x *= value;
        y *= value;
    }

    Vector2<T> operator/(const T value) const
    {
        return Vector2<T>(
            x / value,
            y / value
        );
    }

    void operator/=(const T value)
    {
        x /= value;
        y /= value;
    }


    Vector2 operator*(const Vector2<T> v) const
    {
        return
            x - v.x +
            y - v.y;
    }

    void Multiply(const Vector2<T> v)
    {
		x *= v.x;
		y *= v.y;
    }

    static Vector2<T> Multiply(const Vector2<T> lhs, const Vector2<T> rhs)
    {
		return { lhs.x * rhs.x, lhs.y * rhs.y };
    }

    float GetMagnitude() const { return sqrt(x * x + y * y); }

    Vector2<float> Normalize() const
    {
        if (GetMagnitude() == 0) { return Vector2<T>(); }
        return Vector2<float>(*this / GetMagnitude());
    }

    static T Cross(const Vector2<T> v1, const Vector2<T> v2)
    {
        return (v1.x * v2.y) - (v1.y * v2.x);
    }

    static float GetDistance(const Vector2<T> begin, const Vector2<T> end)
    {
        return (end - begin).GetMagnitude();
    }

    static T GetDistanceManhattan(const Vector2<T> begin, const Vector2<T> end)
    {
        return abs(end.x - begin.x) + abs(end.y - begin.y);
    }

    std::string ToString() const
    {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }

    json ToJson() const
    {
        json vectorJson;
        vectorJson["x"] = x;
        vectorJson["y"] = y;
        return vectorJson;
    }

    void SetFromJson(const json& vectorJson)
    {
        if (CheckJsonExists(vectorJson, "x") && CheckJsonExists(vectorJson, "y")
        ) {
            x = vectorJson["x"];
            y = vectorJson["y"];
        } else if (vectorJson.is_array() && vectorJson.size() >= 2) {
            x = vectorJson[0];
            y = vectorJson[1];
        }
    }

    T x, y;
};

template <typename T>
class Vector3 {
public:
    Vector3()
        : x(static_cast<T>(0)),
          y(static_cast<T>(0)),
          z(static_cast<T>(0)) {}

    Vector3(const T value)
        : x(value),
          y(value),
          z(value) {}

    Vector3(const T x, const T y, const T z)
        : x(x),
          y(y),
          z(z) {}

    template <typename U>
    Vector3(const Vector3<U> v)
        : x(static_cast<T>(v.x)),
          y(static_cast<T>(v.y)),
          z(static_cast<T>(v.z)) {}

    Vector3(const Vector2<T> v, T z = 0)
		: x(v.x),
          y(v.y),
          z(z)
    {
        
    }

    float& operator[](const int index)
    {
        if (index == 0)
            return x;
        else if (index == 1)
            return y;
        else if (index == 2)
            return z;

        cassert(false, "Vector3 out of range!");
    }

    T operator[](const int index) const
    {
        if (index == 0)
            return x;
        else if (index == 1)
            return y;
        else if (index == 2)
            return z;

        cassert(false, "Vector3 out of range!");
    }

    bool operator==(const Vector3<T> v) const
    {
        return x == v.x && y == v.y && z == v.z;
    }

    bool operator!=(const Vector3<T> v) const { return !(*this == v); }

    Vector3<T> operator+(const Vector2<T> v) const
    {
        return Vector3<T>(
            x + v.x,
            y + v.y,
            z
        );
    }

    Vector3<T> operator+(const Vector3<T> v) const
    {
        return Vector3<T>(
            x + v.x,
            y + v.y,
            z + v.z
        );
    }

    void operator+=(const Vector2<T> v)
    {
        x += v.x;
        y += v.y;
    }

    void operator+=(const Vector3<T> v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    Vector3<T> operator-(const Vector2<T> v) const
    {
        return Vector3<T>(
            x - v.x,
            y - v.y,
            z
        );
    }

    Vector3<T> operator-(const Vector3<T> v) const
    {
        return Vector3<T>(
            x - v.x,
            y - v.y,
            z - v.z
        );
    }

    void operator-=(const Vector2<T> v)
    {
        x -= v.x;
        y -= v.y;
    }

    void operator-=(const Vector3<T> v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }

    Vector3<T> operator*(const T value) const
    {
        return Vector3<T>(
            x * value,
            y * value,
            z * value
        );
    }

    void operator*=(const T value)
    {
        x *= value;
        y *= value;
        z *= value;
    }

    Vector3<T> operator/(const T value) const
    {
        return Vector3<T>(
            x / value,
            y / value,
            z / value
        );
    }

    void operator/=(const T value)
    {
        x /= value;
        y /= value;
        z /= value;
    }

    T operator*(const Vector3<T> v) const
    {
        return
            x * v.x +
            y * v.y +
            z * v.z;
    }

    float GetMagnitude() const { return sqrt(x * x + y * y + z * z); }

    static Vector3<T> Multiply(Vector3<T> left, Vector3<T> right)
    {
		return { left.x * right.x, left.y * right.y, left.z * right.z };
    }

	static Vector3<T> Divide(Vector3<T> left, Vector3<T> right)
	{
		return { left.x / right.x, left.y / right.y, left.z / right.z };
	}

    Vector3<float> Normalize() const
    {
        if (GetMagnitude() == 0) { return Vector3<float>(); }
        return Vector3<float>(*this) / GetMagnitude();
    }

    Vector3<T> GetOrthogonal() const
    {
		const float absX = abs(x);
		const float absY = abs(y);
		const float absZ = abs(z);

		const Vector3<T> other = absX < absY ? (absX < absZ ? Vector3<T>(1, 0, 0) : Vector3<T>(0, 0, 1)) : (absY < absZ ? Vector3<T>(0, 1, 0) : Vector3<T>(0, 0, 1));
		return Cross(*this, other);
    }

	static Vector3<T> GetOrthogonal(Vector3<T> v)
	{
		const float x = abs(v.x);
		const float y = abs(v.y);
		const float z = abs(v.z);

		const Vector3<T> other = x < y ? (x < z ? Vector3<T>(1, 0, 0) : Vector3<T>(0, 0, 1)) : (y < z ? Vector3<T>(0, 1, 0) : Vector3<T>(0, 0, 1));
		return Cross(v, other);
	}

	Vector3<T> Cross(const Vector3<T> other) const
    {
		return Vector3<T>(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
			);
    }

    static Vector3<T> Cross(const Vector3<T> v1, const Vector3<T> v2)
    {
        return Vector3<T>(
            v1.y * v2.z - v1.z * v2.y,
            v1.z * v2.x - v1.x * v2.z,
            v1.x * v2.y - v1.y * v2.x
        );
    }

    static float GetDistance(const Vector3<T> begin, const Vector3<T> end)
    {
        return static_cast<float>((end - begin).GetMagnitude());
    }

    static T GetDistanceManhattan(const Vector3<T> begin, const Vector3<T> end)
    {
        return abs(end.x - begin.x) + abs(end.y - begin.y) + abs(
                   end.z - begin.z);
    }

	//constexpr Vector3<T> SmoothDamp(const Vector3<T> &target, const Vector3<T> &rate) const
	//{
	//	//return math::SmoothDamp(*this, target, rate);
	//	return *this + ((target - *this) * rate);
	//}

    std::string ToString() const
    {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::
               to_string(z) + ")";
    }

    json ToJson() const
    {
        json vectorJson;
        vectorJson["x"] = x;
        vectorJson["y"] = y;
        vectorJson["z"] = z;
        return vectorJson;
    }

    void SetFromJson(const json& vectorJson)
    {
        if (CheckJsonExists(vectorJson, "x") && 
			CheckJsonExists(vectorJson, "y") && 
			CheckJsonExists(vectorJson, "z")) {
            x = vectorJson["x"];
            y = vectorJson["y"];
            z = vectorJson["z"];
        } else if (vectorJson.is_array() && vectorJson.size() >= 3) {
            x = vectorJson[0];
            y = vectorJson[1];
            z = vectorJson[2];
        }
    }


    T x, y, z;
};

template <typename T>
class Vector4 {
public:
    Vector4()
        : x(static_cast<T>(0)),
          y(static_cast<T>(0)),
          z(static_cast<T>(0)),
          w(static_cast<T>(0)) {}

    Vector4(const T value)
        : x(value),
          y(value),
          z(value),
          w(value) {}

    Vector4(const T x, const T y, const T z, const T w)
        : x(x),
          y(y),
          z(z),
          w(w) {}

    Vector4(std::array<T, 4>& newArray)
        : x(newArray[0]),
          y(newArray[1]),
          z(newArray[2]),
          w(newArray[3]) {}

    template <typename U>
    Vector4(const Vector4<U> v)
        : x(static_cast<T>(v.x)),
          y(static_cast<T>(v.y)),
          z(static_cast<T>(v.z)),
          w(static_cast<T>(v.w)) {}

    T& operator[](const int index)
    {
        if (index == 0)
            return x;
        else if (index == 1)
            return y;
        else if (index == 2)
            return z;
        else if (index == 3)
            return w;

        cassert(false, "Vector4 out of range!");
    }

    T operator[](const int index) const
    {
        if (index == 0)
            return x;
        else if (index == 1)
            return y;
        else if (index == 2)
            return z;
        else if (index == 3)
            return w;

        cassert(false, "Vector4 out of range!");
    }


    bool operator==(const Vector4<T> v) const
    {
        return x == v.x && y == v.y && z == v.z && w == v.w;
    }

    bool operator!=(const Vector4<T>& v) const { return !(*this == v); }

    Vector4<T> operator+(const Vector2<T> v) const
    {
        return Vector4<T>(
            x + v.x,
            y + v.y,
            z,
            w
        );
    }

    Vector4<T> operator+(const Vector3<T> v) const
    {
        return Vector4<T>(
            x + v.x,
            y + v.y,
            z + v.z,
            w
        );
    }

    Vector4<T> operator+(const Vector4<T> v) const
    {
        return Vector4<T>(
            x + v.x,
            y + v.y,
            z + v.z,
            w + v.w
        );
    }

    void operator+=(const Vector2<T> v)
    {
        x += v.x;
        y += v.y;
    }

    void operator+=(const Vector3<T> v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    void operator+=(const Vector4<T> v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
    }

    Vector4<T> operator-(const Vector2<T> v) const
    {
        return Vector4<T>(
            x - v.x,
            y - v.y,
            z,
            w
        );
    }

    Vector4<T> operator-(const Vector3<T> v) const
    {
        return Vector4<T>(
            x - v.x,
            y - v.y,
            z - v.z,
            w
        );
    }

    Vector4<T> operator-(const Vector4<T> v) const
    {
        return Vector4<T>(
            x - v.x,
            y - v.y,
            z - v.z,
            w - v.w
        );
    }

    void operator-=(const Vector2<T> v)
    {
        x -= v.x;
        y -= v.y;
    }

    void operator-=(const Vector3<T> v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }

    void operator-=(const Vector4<T> v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
    }

    Vector4<T> operator*(const T value) const
    {
        return Vector4<T>(
            x * value,
            y * value,
            z * value,
            w * value
        );
    }

    void operator*=(const T value)
    {
        x *= value;
        y *= value;
        z *= value;
        w *= value;
    }

    Vector4<T> operator/(const T value) const
    {
        return Vector4<T>(
            x / value,
            y / value,
            z / value,
            w / value
        );
    }

    void operator/=(const T value)
    {
        x /= value;
        y /= value;
        z /= value;
        w /= value;
    }

    T operator*(const Vector4<T> v) const
    {
        return x * v.x +
            y * v.y +
            z * v.z +
            w * v.w;
    }

	float GetMagnitude() const { return sqrt(x * x + y * y + z * z + w * w); }

	Vector4<T> Normalize() const
	{
		if (GetMagnitude() == 0) { return Vector4<T>(); }
		return Vector4<T>(*this) / GetMagnitude();
	}

    std::string ToString() const
    {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::
               to_string(z) + ", " + std::to_string(w) + ")";
    }

    json ToJson() const
    {
        json vectorJson;
        vectorJson["x"] = x;
        vectorJson["y"] = y;
        vectorJson["z"] = z;
        vectorJson["w"] = w;
        return vectorJson;
    }

    void SetFromJson(const json& vectorJson)
    {
        if (CheckJsonExists(vectorJson, "x") &&
            CheckJsonExists(vectorJson, "y") &&
            CheckJsonExists(vectorJson, "z") &&
            CheckJsonExists(vectorJson, "w")
        ) {
            x = vectorJson["x"];
            y = vectorJson["y"];
            z = vectorJson["z"];
            w = vectorJson["w"];
        } else if (vectorJson.is_array() && vectorJson.size() >= 4) {
            x = vectorJson[0];
            y = vectorJson[1];
            z = vectorJson[2];
            w = vectorJson[3];
        }
    }

    Vector3<T> To3()
    {
		return { x, y, z };
    }

    T x, y, z, w;
};                       

using Vec2 = Vector2<float>;
using Vec2Char = Vector2<char>;
using Vec2UChar = Vector2<unsigned char>;
using Vec2Short = Vector2<short>;
using Vec2UShort = Vector2<unsigned short>;
using Vec2Int = Vector2<int>;
using Vec2UInt = Vector2<unsigned int>;
using Vec2Long = Vector2<long>;
using Vec2ULong = Vector2<unsigned long>;
using Vec2LongLong = Vector2<long long>;
using Vec2ULongLong = Vector2<unsigned long long>;

using Vec3 = Vector3<float>;
using Vec3Char = Vector3<char>;
using Vec3UChar = Vector3<unsigned char>;
using Vec3Short = Vector3<short>;
using Vec3UShort = Vector3<unsigned short>;
using Vec3Int = Vector3<int>;
using Vec3UInt = Vector3<unsigned int>;
using Vec3Long = Vector3<long>;
using Vec3ULong = Vector3<unsigned long>;
using Vec3LongLong = Vector3<long long>;
using Vec3ULongLong = Vector3<unsigned long long>;

using Vec4 = Vector4<float>;
using Vec4Char = Vector4<char>;
using Vec4UChar = Vector4<unsigned char>;
using Vec4Short = Vector4<short>;
using Vec4UShort = Vector4<unsigned short>;
using Vec4Int = Vector4<int>;
using Vec4UInt = Vector4<unsigned int>;
using Vec4Long = Vector4<long>;
using Vec4ULong = Vector4<unsigned long>;
using Vec4LongLong = Vector4<long long>;
using Vec4ULongLong = Vector4<unsigned long long>;
} //namespace poke::math
