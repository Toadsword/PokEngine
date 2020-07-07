#include <Math/matrix.h>
#include <string>
#include <cmath>

namespace poke::math {
#pragma region Matrix 2x2
Matrix2::Matrix2()
{
    matrix_ = {Vec2(0, 0), Vec2(0, 0) };
}

Matrix2::Matrix2(const float value)
{
    matrix_ = {Vec2(value, value), Vec2(value, value)};
}

Matrix2::Matrix2(const std::array<Vec2, 2> newMatrix) { matrix_ = newMatrix; }

Matrix2::Matrix2(const Vec2 v1, const Vec2 v2)
{
    matrix_ = {v1, v2 };
}

Vec2 Matrix2::operator[](const int index) const { return matrix_[index]; }

Vec2& Matrix2::operator[](const int index) { return matrix_[index]; }


void Matrix2::SetValue(const int x, const int y, const float value) { matrix_[x][y] = value; }

Vec2 Matrix2::GetColumn(const int index) const { return matrix_[index]; }

void Matrix2::SetColumn(const int index, const Vec2 newValues) { matrix_[index] = newValues; }


Matrix2 Matrix2::Identity() { return Matrix2(Vec2(1, 0), Vec2(0, 1)); }

std::string Matrix2::ToString() const
{
    return
        "| " + std::to_string(matrix_[0].x) + " | " + std::
        to_string(matrix_[1].x) + " |" + "\n" +
        "| " + std::to_string(matrix_[0].y) + " | " + std::
        to_string(matrix_[1].y) + " |";
}

Matrix2 Matrix2::operator*(const Matrix2 matrix2) const
{
    Matrix2 result(0.0f);

    for (int i = 0; i < kMatrixDimension; i++) {
        for (int j = 0; j < kMatrixDimension; j++) {
            result.SetValue(
                i,
                j,
                matrix_[0][j] * matrix2[i][0] +  matrix_[1][j] * matrix2[i][1]
            );
        }
    }
    return result;
}

Vec2 Matrix2::operator*(const Vec2 vector2) const
{
    return Vec2(
        matrix_[0][0] * vector2.x + matrix_[1][0] * vector2.y,
        matrix_[0][1] * vector2.x + matrix_[1][1] * vector2.y
    );
}
#pragma endregion

#pragma region Matrix 3x3
Matrix3::Matrix3()
{
    matrix_ = {Vec3(0, 0, 0),Vec3(0, 0, 0),Vec3(0, 0, 0) };
}

Matrix3::Matrix3(const float value)
{
    matrix_ = {
        Vec3(value, value, value),
        Vec3(value, value, value),
        Vec3(value, value, value) 
    };
}

Matrix3::Matrix3(std::array<Vec3, kMatrixDimension> const newMatrix) { matrix_ = newMatrix; }

Matrix3::Matrix3(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
    matrix_ = {
        v1,
        v2,
        v3
    };
}

Vec3 Matrix3::operator[](const int index) const { return matrix_[index]; }

Vec3& Matrix3::operator[](const int index) { return matrix_[index]; }

void Matrix3::SetValue(const int x, const int y, const float value) { matrix_[x][y] = value; }

Vec3 Matrix3::GetColumn(const int index) const { return matrix_[index]; }

void Matrix3::SetColumn(const int index, const Vec3 newValues) { matrix_[index] = newValues; }

Matrix3 Matrix3::Identity() { return Matrix3(Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1)); }

Matrix3 Matrix3::operator*(const Matrix3 matrix3) const
{
    Matrix3 result(0.0f);

    for (int i = 0; i < kMatrixDimension; i++) {
        for (int j = 0; j < kMatrixDimension; j++) {
            result.SetValue(
                i,
                j,
                matrix_[0][j] * matrix3[i][0] +
                matrix_[1][j] * matrix3[i][1] +
                matrix_[2][j] * matrix3[i][2]
            );
        }
    }
    return result;
}

Vec3 Matrix3::operator*(const Vec3 vector3) const
{
    return Vec3(
        matrix_[0][0] * vector3.x + matrix_[1][0] * vector3.y + matrix_[
            2][0] * vector3.z,
        matrix_[0][1] * vector3.x + matrix_[1][1] * vector3.y + matrix_[
            2][1] * vector3.z,
        matrix_[0][2] * vector3.x + matrix_[1][2] * vector3.y + matrix_[
            2][2] * vector3.z
    );
}

std::string Matrix3::ToString() const
{
    const Vec3 v1 = GetColumn(0);
    const Vec3 v2 = GetColumn(1);
    const Vec3 v3 = GetColumn(2);

    return
        "| " + std::to_string(v1.x) + " | " + std::to_string(v2.x) + " | " + std
        ::to_string(v3.x) + " |" + "\n" +
        "| " + std::to_string(v1.y) + " | " + std::to_string(v2.y) + " | " + std
        ::to_string(v3.y) + " |" + "\n" +
        "| " + std::to_string(v1.z) + " | " + std::to_string(v2.z) + " | " + std
        ::to_string(v3.z) + " |";
}

Matrix3 Matrix3::GetInverse(Matrix3 matrix)
{
    Matrix3 result;
    float determinant = 0;
    for (int i = 0; i < kMatrixDimension; i++)
        determinant = determinant + matrix[0][i] * (
                          matrix[1][(i + 1) % kMatrixDimension] * matrix[2][
                              (i + 2) % kMatrixDimension] - matrix[1][(i + 2) % kMatrixDimension] *
                          matrix[2][(i + 1) % kMatrixDimension]);

    for (int i = 0; i < kMatrixDimension; i++) {
        for (int j = 0; j < kMatrixDimension; j++)
            result[i][j] = (matrix[(j + 1) % kMatrixDimension][(i + 1) % kMatrixDimension] * matrix[
                                (j + 2) % kMatrixDimension][(i + 2) % kMatrixDimension] - matrix[
                                (j + 1) % kMatrixDimension][(i + 2) % kMatrixDimension] * matrix[
                                (j + 2) % kMatrixDimension][(i + 1) % kMatrixDimension]) /
                           determinant;
    }

	std::cout << "det = " << determinant << "\n";

    return result;
}
#pragma endregion

#pragma region Matrix4x4
Matrix4::Matrix4()
{
    matrix_ = {
        Vec4(0, 0, 0, 0),
        // column 1
        Vec4(0, 0, 0, 0),
        // column 2
        Vec4(0, 0, 0, 0),
        // column 3
        Vec4(0, 0, 0, 0) // column 4
    };
}

Matrix4::Matrix4(const float value)
{
    matrix_ = {
        Vec4(value, value, value, value),
        // column 1
        Vec4(value, value, value, value),
        // column 2
        Vec4(value, value, value, value),
        // column 3
        Vec4(value, value, value, value) // column 4
    };
}

Matrix4::Matrix4(std::array<Vec4, kMatrixDimension> const newMatrix) { matrix_ = newMatrix; }

Matrix4::Matrix4(const Vec4 v1, const Vec4 v2, const Vec4 v3, const Vec4 v4)
{
    matrix_ = {
        v1,
        // column 1
        v2,
        // column 2
        v3,
        // column 3		   
        v4 // column 4
    };
}

Vec4 Matrix4::operator[](const int index) const { return matrix_[index]; }

Vec4& Matrix4::operator[](const int index) { return matrix_[index]; }

void Matrix4::SetValue(const int x, const int y, const float value) { matrix_[x][y] = value; }

Vec4 Matrix4::GetColumn(const int index) const { return matrix_[index]; }

void Matrix4::SetColumn(const int index, const Vec4 newValues) { matrix_[index] = newValues; }

Matrix4 Matrix4::Identity()
{
    return Matrix4(
        Vec4(1, 0, 0, 0),
        Vec4(0, 1, 0, 0),
        Vec4(0, 0, 1, 0),
        Vec4(0, 0, 0, 1));
}

Matrix4 Matrix4::operator*(const Matrix4 matrix4) const
{
    Matrix4 result(0.0f);

    for (int i = 0; i < kMatrixDimension; i++) {
        for (int j = 0; j < kMatrixDimension; j++) {
            result.SetValue(
                i,
                j,
                matrix_[0][j] * matrix4[i][0] +
                matrix_[1][j] * matrix4[i][1] +
                matrix_[2][j] * matrix4[i][2] +
                matrix_[3][j] * matrix4[i][3]
            );
        }
    }
    return result;
}

Vec4 Matrix4::operator*(const Vec4 vector4) const
{
    return Vec4(
        matrix_[0][0] * vector4.x + matrix_[1][0] * vector4.y + matrix_[2][0] * vector4.z + matrix_[
            3][0] * vector4.w,
        matrix_[0][1] * vector4.x + matrix_[1][1] * vector4.y + matrix_[2][1] * vector4.z + matrix_[
            3][1] * vector4.w,
        matrix_[0][2] * vector4.x + matrix_[1][2] * vector4.y + matrix_[2][2] * vector4.z + matrix_[
            3][2] * vector4.w,
        matrix_[0][3] * vector4.x + matrix_[1][3] * vector4.y + matrix_[2][3] * vector4.z + matrix_[
            3][3] * vector4.w
    );
}

Matrix4 Matrix4::operator*(const float f) const
{
    Matrix4 result;
    for (int i = 0; i < kMatrixDimension; i++) {
        for (int j = 0; j < kMatrixDimension; j++) { result[i][j] = matrix_[i][j] * f; }
    }
    return result;
}

Matrix3 Matrix4::GetSubMatrix3()
{
    return Matrix3(
        Vec3(matrix_[0][0], matrix_[0][1], matrix_[0][2]),
        Vec3(matrix_[1][0], matrix_[1][1], matrix_[1][2]),
        Vec3(matrix_[2][0], matrix_[2][1], matrix_[2][2])
    );
}


std::string Matrix4::ToString() const
{
    Vec4 v1 = GetColumn(0);
    Vec4 v2 = GetColumn(1);
    Vec4 v3 = GetColumn(2);
    Vec4 v4 = GetColumn(3);

    return
        "| " + std::to_string(v1.x) + " | " + std::to_string(v2.x) + " | " + std
        ::to_string(v3.x) + " | " + std::to_string(v4.x) + " |" + +"\n" +
        "| " + std::to_string(v1.y) + " | " + std::to_string(v2.y) + " | " + std
        ::to_string(v3.y) + " | " + std::to_string(v4.y) + " |" + +"\n" +
        "| " + std::to_string(v1.z) + " | " + std::to_string(v2.z) + " | " + std
        ::to_string(v3.z) + " | " + std::to_string(v4.z) + " |" + +"\n" +
        "| " + std::to_string(v1.w) + " | " + std::to_string(v2.w) + " | " + std
        ::to_string(v3.w) + " | " + std::to_string(v4.w) + " |";
}

Matrix4 Matrix4::Rotate(
    const Matrix4 modelMatrix,
    const float angle,
    const Vec3& axis)
{
    const float angleCos = cos(angle);
    const float angleSin = sin(angle);

    const Vec3 axisNorma = axis.Normalize();
    const Vec3 temp = axisNorma * (1.0f - angleCos);

    Matrix4 rotate(0);

    rotate[0][0] = angleCos + temp.x * axisNorma.x;
    rotate[0][1] = temp.x * axisNorma.y + angleSin * axisNorma.z;
    rotate[0][2] = temp.x * axisNorma.z - angleSin * axisNorma.y;

    rotate[1][0] = temp.y * axisNorma.x - angleSin * axisNorma.z;
    rotate[1][1] = angleCos + temp.y * axisNorma.y;
    rotate[1][2] = temp.y * axisNorma.z + angleSin * axisNorma.x;

    rotate[2][0] = temp.z * axisNorma.x + angleSin * axisNorma.y;
    rotate[2][1] = temp.z * axisNorma.y - angleSin * axisNorma.x;
    rotate[2][2] = angleCos + temp.z * axisNorma.z;

    Matrix4 result(0);
    result[0] = modelMatrix[0] * rotate[0][0] + modelMatrix[1] * rotate[0][1] +
                modelMatrix[2] * rotate[0][2];
    result[1] = modelMatrix[0] * rotate[1][0] + modelMatrix[1] * rotate[1][1] +
                modelMatrix[2] * rotate[1][2];
    result[2] = modelMatrix[0] * rotate[2][0] + modelMatrix[1] * rotate[2][1] +
                modelMatrix[2] * rotate[2][2];
    result[3] = modelMatrix[3];

    result.SetColumn(3, modelMatrix.GetColumn(3));

    return result;
}

Matrix4 Matrix4::Translate(const Matrix4 modelMatrix, const Vec3 position)
{
    Vec4 translate(0, 0, 0, 0);
    translate =
        modelMatrix[0] * position.x +
        modelMatrix[1] * position.y +
        modelMatrix[2] * position.z +
        modelMatrix[3];

    Matrix4 result(modelMatrix);

    result.SetColumn(3, translate);

    return result;
}

Vec4 Matrix4::Transform(const Matrix4 modelMatrix, const Vec4 position)
{
    Vec4 result;

    for (uint32_t row = 0; row < 4; row++) {
        result[row] = modelMatrix[0][row] * position.x + modelMatrix[1][row] *
                      position.y + modelMatrix[2][row] * position.z +
                      modelMatrix[3][row] * position.w;
    }

    return result;
}

Matrix4 Matrix4::Scale(const Matrix4 modelMatrix, const Vec3 scale)
{
    Matrix4 result = Identity();
    result.SetColumn(0, modelMatrix.GetColumn(0) * scale.x);
    result.SetColumn(1, modelMatrix.GetColumn(1) * scale.y);
    result.SetColumn(2, modelMatrix.GetColumn(2) * scale.z);
    result.SetColumn(3, modelMatrix.GetColumn(3));

    return result;
}

Matrix4 Matrix4::LookAt(const Vec3 eye, const Vec3 center, const Vec3 up)
{
    const Vec3 f((center - eye).Normalize());
    const Vec3 s(Vec3::Cross(f, up).Normalize());
    const Vec3 u(Vec3::Cross(s, f));

    Matrix4 result = Identity();
    result[0][0] = s.x;
    result[1][0] = s.y;
    result[2][0] = s.z;
    result[0][1] = u.x;
    result[1][1] = u.y;
    result[2][1] = u.z;
    result[0][2] = -f.x;
    result[1][2] = -f.y;
    result[2][2] = -f.z;
    result[3][0] = -(s * eye);
    result[3][1] = -(u * eye);
    result[3][2] = (f * eye);

    return result;
}

Matrix4 Matrix4::Perspective(
    const float fov,
    const float aspect,
    const float near,
    const float far)
{
    const float tanHalfFov = tan(fov * 0.5f* (3.14f / 180));

    Matrix4 result(0);

    result[0][0] = 1.0f / (aspect * tanHalfFov);
    result[1][1] = 1.0f / tanHalfFov;
    result[2][2] = far / (near - far);
    result[2][3] = -1.0f;
    result[3][2] = -2 * (far * near) / (far - near);

    return result;
}

Matrix4 Matrix4::GetWorldMatrix(
    const Vec3 position,
    const Vec3 rotation,
    const Vec3 scale)
{
    Matrix4 worldMatrix = Identity();
    worldMatrix = Translate(worldMatrix, position);

    worldMatrix = Rotate(worldMatrix, rotation.x, Vec3(1, 0, 0));
    worldMatrix = Rotate(worldMatrix, rotation.y, Vec3(0, 1, 0));
    worldMatrix = Rotate(worldMatrix, rotation.z, Vec3(0, 0, 1));

    worldMatrix = Scale(worldMatrix, scale);

    return worldMatrix;
}

Matrix4 Matrix4::GetTranspose(const Matrix4& matrix4)
{
    return {
        {matrix4[0][0], matrix4[1][0], matrix4[2][0], matrix4[3][0]},
        {matrix4[0][1], matrix4[1][1], matrix4[2][1], matrix4[3][1]},
        {matrix4[0][2], matrix4[1][2], matrix4[2][2], matrix4[3][2]},
        {matrix4[0][3], matrix4[1][3], matrix4[2][3], matrix4[3][3]}
    };
}

Matrix4 Matrix4::GetInverse(const Matrix4& matrix4)
{
	//
	// Inversion by Cramer's rule.  Code taken from an Intel publication
	//
    double Result[4][4];
    double tmp[12]; /* temp array for pairs */
    double src[16]; /* array of transpose source matrix */
    double det; /* determinant */
    /* transpose matrix */
    for (int i = 0; i < 4; i++)
    {
	    src[i + 0] = matrix4[i][0];
	    src[i + 4] = matrix4[i][1];
	    src[i + 8] = matrix4[i][2];
	    src[i + 12] =matrix4[i][3];
    }
    /* calculate pairs for first 8 elements (cofactors) */
    tmp[0] = src[10] * src[15];
    tmp[1] = src[11] * src[14];
    tmp[2] = src[9] * src[15];
    tmp[3] = src[11] * src[13];
    tmp[4] = src[9] * src[14];
    tmp[5] = src[10] * src[13];
    tmp[6] = src[8] * src[15];
    tmp[7] = src[11] * src[12];
    tmp[8] = src[8] * src[14];
    tmp[9] = src[10] * src[12];
    tmp[10] = src[8] * src[13];
    tmp[11] = src[9] * src[12];
    /* calculate first 8 elements (cofactors) */
    Result[0][0] = tmp[0] * src[5] + tmp[3] * src[6] + tmp[4] * src[7];
    Result[0][0] -= tmp[1] * src[5] + tmp[2] * src[6] + tmp[5] * src[7];
    Result[0][1] = tmp[1] * src[4] + tmp[6] * src[6] + tmp[9] * src[7];
    Result[0][1] -= tmp[0] * src[4] + tmp[7] * src[6] + tmp[8] * src[7];
    Result[0][2] = tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7];
    Result[0][2] -= tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7];
    Result[0][3] = tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6];
    Result[0][3] -= tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6];
    Result[1][0] = tmp[1] * src[1] + tmp[2] * src[2] + tmp[5] * src[3];
    Result[1][0] -= tmp[0] * src[1] + tmp[3] * src[2] + tmp[4] * src[3];
    Result[1][1] = tmp[0] * src[0] + tmp[7] * src[2] + tmp[8] * src[3];
    Result[1][1] -= tmp[1] * src[0] + tmp[6] * src[2] + tmp[9] * src[3];
    Result[1][2] = tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3];
    Result[1][2] -= tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3];
    Result[1][3] = tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2];
    Result[1][3] -= tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2];
    /* calculate pairs for second 8 elements (cofactors) */
    tmp[0] = src[2] * src[7];
    tmp[1] = src[3] * src[6];
    tmp[2] = src[1] * src[7];
    tmp[3] = src[3] * src[5];
    tmp[4] = src[1] * src[6];
    tmp[5] = src[2] * src[5];

    tmp[6] = src[0] * src[7];
    tmp[7] = src[3] * src[4];
    tmp[8] = src[0] * src[6];
    tmp[9] = src[2] * src[4];
    tmp[10] = src[0] * src[5];
    tmp[11] = src[1] * src[4];
    /* calculate second 8 elements (cofactors) */
    Result[2][0] = tmp[0] * src[13] + tmp[3] * src[14] + tmp[4] * src[15];
    Result[2][0] -= tmp[1] * src[13] + tmp[2] * src[14] + tmp[5] * src[15];
    Result[2][1] = tmp[1] * src[12] + tmp[6] * src[14] + tmp[9] * src[15];
    Result[2][1] -= tmp[0] * src[12] + tmp[7] * src[14] + tmp[8] * src[15];
    Result[2][2] = tmp[2] * src[12] + tmp[7] * src[13] + tmp[10] * src[15];
    Result[2][2] -= tmp[3] * src[12] + tmp[6] * src[13] + tmp[11] * src[15];
    Result[2][3] = tmp[5] * src[12] + tmp[8] * src[13] + tmp[11] * src[14];
    Result[2][3] -= tmp[4] * src[12] + tmp[9] * src[13] + tmp[10] * src[14];
    Result[3][0] = tmp[2] * src[10] + tmp[5] * src[11] + tmp[1] * src[9];
    Result[3][0] -= tmp[4] * src[11] + tmp[0] * src[9] + tmp[3] * src[10];
    Result[3][1] = tmp[8] * src[11] + tmp[0] * src[8] + tmp[7] * src[10];
    Result[3][1] -= tmp[6] * src[10] + tmp[9] * src[11] + tmp[1] * src[8];
    Result[3][2] = tmp[6] * src[9] + tmp[11] * src[11] + tmp[3] * src[8];
    Result[3][2] -= tmp[10] * src[11] + tmp[2] * src[8] + tmp[7] * src[9];
    Result[3][3] = tmp[10] * src[10] + tmp[4] * src[8] + tmp[9] * src[9];
    Result[3][3] -= tmp[8] * src[9] + tmp[11] * src[10] + tmp[5] * src[8];
    /* calculate determinant */
    det = src[0] * Result[0][0] + src[1] * Result[0][1] + src[2] * Result[0][2] + src[3] * Result[0][3];
    /* calculate matrix inverse */
    det = 1.0f / det;

    Matrix4 FloatResult;
    for (int i = 0; i < 4; i++)
    {
	    for (int j = 0; j < 4; j++)
	    {
		    FloatResult[i][j] = float(Result[i][j] * det);
	    }
    }
    return FloatResult;
}

Matrix4 Matrix4::Rotate(const float angle, const Vec3 axis)
{
    Matrix4 result;

    auto c = std::cos(angle);
    auto s = std::sin(angle);
    auto o = 1.0f - c;
    auto xy = axis.x * axis.y;
    auto yz = axis.y * axis.z;
    auto xz = axis.x * axis.z;
    auto xs = axis.x * s;
    auto ys = axis.y * s;
    auto zs = axis.z * s;

    Matrix3 f;
    f[0][0] = axis.x * axis.x * o + c;
    f[0][1] = xy * o + zs;
    f[0][2] = xz * o - ys;
    f[1][0] = xy * o - zs;
    f[1][1] = axis.y * axis.y * o + c;
    f[1][2] = yz * o + xs;
    f[2][0] = xz * o + ys;
    f[2][1] = yz * o - xs;
    f[2][2] = axis.z * axis.z * o + c;

    for (uint32_t row = 0; row < 3; row++) {
        for (uint32_t col = 0; col < 4; col++) {
            result[row][col] = matrix_[0][col] * f[row][0] + matrix_[1][col] * f[row][1] + matrix_[2
                               ][col] * f[row][2];
        }
    }

    result[3] = matrix_[3];
    return result;
}

Matrix4 Matrix4::ViewMatrix(const Vec3 position, const Vec3 rotation)
{
    Matrix4 result = Matrix4::Identity();

    result = Rotate(result, rotation.x, {1.0f, 0.0f, 0.0f});
    result = Rotate(result, rotation.y, {0.0f, 1.0f, 0.0f});
    result = Rotate(result, rotation.z, {0.0f, 0.0f, 1.0f});
    result = Translate(result, {-position.x, -position.y, -position.z});
    return result;
}

Matrix4 Matrix4::PerspectiveMatrix(
    const float fov,
    const float aspectRatio,
    const float zNear,
    const float zFar)
{
    Matrix4 result(0.0f);

    const auto f = std::tan(0.5f * fov);

    result[0][0] = 1.0f / (aspectRatio * f);
    result[1][1] = 1.0f / f;
    result[2][2] = zFar / (zNear - zFar);
    result[2][3] = -1.0f;
    result[3][2] = -(zFar * zNear) / (zFar - zNear);
    return result;
}
#pragma endregion
} //namespace poke::math
