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

#include <array>

#include <Math/vector.h>

namespace poke::math
{
class Matrix2
{
private:
	inline const static int kMatrixDimension = 2;
public:
	Matrix2();
	Matrix2(float value);
	Matrix2(std::array<Vec2, kMatrixDimension> newMatrix);
	Matrix2(Vec2 v1, Vec2 v2);

	Vec2 operator[](int index) const;
	Vec2& operator[](int index);

	void SetValue(int x, int y, float value);
	
	Vec2 GetColumn(int index) const;
	void SetColumn(int index, Vec2 newValues);

	static Matrix2 Identity();

	std::string ToString() const;

	Matrix2 operator* (Matrix2 matrix2) const;

	Vec2 operator* (Vec2 vector2) const;

private:
	std::array<Vec2, kMatrixDimension> matrix_;
};

class Matrix3 {
private:
	const static int kMatrixDimension = 3;
public:
	Matrix3();
	Matrix3(float value);
	Matrix3(std::array<Vec3, kMatrixDimension> newMatrix);
	Matrix3(const Vec3& v1, const Vec3& v2, const Vec3& v3);

	Vec3 operator[](int index) const;
	Vec3& operator[](int index);

	void SetValue(int x, int y, float value);

	Vec3 GetColumn(int index) const;
	void SetColumn(int index, Vec3 newValues);

	static Matrix3 Identity();

	Matrix3 operator*(Matrix3 matrix3) const;
	Vec3 operator*(Vec3 vector3) const;

	std::string ToString() const;

    static Matrix3 GetInverse(Matrix3 matrix);

private:
	std::array<Vec3, kMatrixDimension> matrix_;
};

class Matrix4 {
private:
	const static int kMatrixDimension = 4;
public:
	Matrix4();
	Matrix4(float value);
	Matrix4(std::array<Vec4, kMatrixDimension> newMatrix);
	Matrix4(Vec4 v1, Vec4 v2, Vec4 v3, Vec4 v4);
	
	Vec4 operator[](int index) const;
	Vec4& operator[](int index);

	void SetValue(int x, int y, float value);

	Vec4 GetColumn(int index) const;
	void SetColumn(int index, Vec4 newValues);

	static Matrix4 Identity();

	Matrix4 operator*(Matrix4 matrix4) const;

	Vec4 operator*(Vec4 vector4) const;

    Matrix4 operator*(float f) const;

	Matrix3 GetSubMatrix3();

	std::string ToString() const;

	/**
	 * \brief : Rotate your model matrix
	 * \param modelMatrix
	 * \param angle : In radian
	 * \param axis  : The axis that you want to affect
	 */
	static Matrix4 Rotate(Matrix4 modelMatrix, float angle, const Vec3& axis);
	Matrix4 Rotate(float angle, const Vec3 axis);

	/**
	 * \brief : Set a new position în the model matrix
	 * \param modelMatrix
	 * \param position : The new position
	 */
	static Matrix4 Translate(Matrix4 modelMatrix, Vec3 position);

	/**
	 * \brief : Transforms this matrix by a vector.
	 * \param modelMatrix
	 * \param position 
	 */
    static Vec4 Transform(Matrix4 modelMatrix, Vec4 position);

	/**
	 * \brief : Set the scale in your model matrix
	 * \param modelMatrix
	 * \param scale : the new scale
	 */
	static Matrix4 Scale(Matrix4 modelMatrix, Vec3 scale);

	/**
	 * \brief Create a matrix for the view
	 * \param eye : The position of the camera
	 * \param center : the position where the camera want to look at
	 * \param up : the vector up that you will use as a up 
	 */
	static Matrix4 LookAt(Vec3 eye, Vec3 center, Vec3 up = Vec3(0, 1, 0));
	
	/**
	 * \brief Create a matrix for a perspective view frustum
	 * \param fov : the Field of view
	 * \param aspect : The aspect ratio that determine the field of view in the x direction
	 * \param near : The closest distance of the view
	 * \param far : The furthest distance of the view
	 * \return : the projection matrix
	 */
	static Matrix4 Perspective(float fov, float aspect, float near, float far);

	static Matrix4 GetWorldMatrix(Vec3 position, Vec3 rotation, Vec3 scale);

    /**
     * \brief Get the transpose of the given matrix
     * \param matrix4 
     * \return 
     */
    static Matrix4 GetTranspose(const Matrix4& matrix4);

    /**
     * \brief Get the inverse of the given matrix
     * \param matrix4 
     * \return 
     */
    static Matrix4 GetInverse(const Matrix4& matrix4);

    static Matrix4 ViewMatrix(const math::Vec3 position, const math::Vec3 rotation);

    static Matrix4 PerspectiveMatrix(float fov, float aspectRatio, float zNear, float zFar);

private:
	std::array<Vec4, kMatrixDimension> matrix_;
};
}//namespace poke::math