#include <Math/math.h>
#include <Math/quaternion.h>

namespace poke::math {
//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------
Quaternion::Quaternion()
    : x(0),
      y(0),
      z(0),
      w(0) { }

Quaternion::Quaternion(const float value)
    : x(value),
      y(value),
      z(value),
      w(value) { }

Quaternion::Quaternion(const Vec3 v, const float w)
    : w(w)
{
    x = v.x;
    y = v.y;
    z = v.z;
}

Quaternion::Quaternion(
    const float x,
    const float y,
    const float z,
    const float w)
    : x(x),
      y(y),
      z(z),
      w(w) { }

//-----------------------------------------------------------------------------
// Formulas
//-----------------------------------------------------------------------------
void Quaternion::Normalize() { *this = *this / Magnitude(); }

Quaternion Quaternion::Normalize(Quaternion q)
{
    q.Normalize();
    return q;
}

float Quaternion::Magnitude() const
{
    return sqrt(
        x * x +
        y * y +
        z * z +
        w * w);
}

float Quaternion::Dot(const Quaternion lhs, const Quaternion rhs)
{
    return lhs.x * rhs.x +
           lhs.y * rhs.y +
           lhs.z * rhs.z +
           lhs.w * rhs.w;
}

Matrix3 Quaternion::ToRotationMatrix3() const
{
    return Matrix3(
        Vec3(
            1.0f - 2.0f * (y * y) -
            2.0f * (z * z),
            2.0f * x * y -
            2.0f * z * w,
            2.0f * x * z +
            2.0f * y * w),
        Vec3(
            2.0f * x * y +
            2.0f * z * w,
            1.0f - 2.0f * (x * x) -
            2.0f * (z * z),
            2.0f * y * z -
            2.0f * x * w),
        Vec3(
            2.0f * x * z -
            2.0f * y * w,
            2.0f * y * z +
            2.0f * x * w,
            1.0f - 2.0f * (x * x) -
            2.0f * (y * y)));
}

Matrix4 Quaternion::ToRotationMatrix4() const
{
    return Matrix4(
        Vec4(
            1.0f - 2.0f * (y * y) +
            2.0f * (z * z),
            2.0f * x * y +
            2.0f * z * w,
            2.0f * x * z -
            2.0f * y * w,
            0.0f),
        Vec4(
            2.0f * x * y -
            2.0f * z * w,
            1.0f - 2.0f * (x * x) -
            2.0f * (z * z),
            2.0f * y * z +
            2.0f * x * w,
            0.0f),
        Vec4(
            2.0f * x * z +
            2.0f * y * w,
            2.0f * y * z -
            2.0f * x * w,
            1.0f - 2.0f * (x * x) +
            2.0f * (y * y),
            0.0f),
        Vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

Quaternion Quaternion::GetConjugate() const
{
    return Quaternion(
        -x,
        -y,
        -z,
        w);
}

Quaternion Quaternion::GetInverse() const
{
    const Quaternion conj = GetConjugate();
    const float mag = Magnitude();

    return conj / (mag * mag);
}

Quaternion Quaternion::RotateAroundAxis(const Vec3 rhs, const float rot)
{
    const float factor = sinf(rot / 2.0f);

    const float x = rhs.x * factor;
    const float y = rhs.y * factor;
    const float z = rhs.z * factor;

    const float w = cosf(rot / 2.0f);

    return Quaternion(x, y, z, w);
}

Vec3 Quaternion::GetEulerAngles() const
{
    return ToEulerRad(*this);
}

Quaternion Quaternion::FromEulerRad(const math::Vec3 euler) {
	const float yaw = euler.x;
	const float pitch = euler.y;
	const float roll = euler.z;
	const float rollOver2 = roll * 0.5f;
	const float sinRollOver2 = sin(rollOver2);
	const float cosRollOver2 = cos(rollOver2);
	const float pitchOver2 = pitch * 0.5f;
	const float sinPitchOver2 = sin(pitchOver2);
	const float cosPitchOver2 = cos(pitchOver2);
	const float yawOver2 = yaw * 0.5f;
	const float sinYawOver2 = sin(yawOver2);
	const float cosYawOver2 = cos(yawOver2);
	math::Quaternion result;
	result.x = cosYawOver2 * cosPitchOver2 * cosRollOver2 + sinYawOver2 * sinPitchOver2 * sinRollOver2;
	result.y = cosYawOver2 * cosPitchOver2 * sinRollOver2 - sinYawOver2 * sinPitchOver2 * cosRollOver2;
	result.z = cosYawOver2 * sinPitchOver2 * cosRollOver2 + sinYawOver2 * cosPitchOver2 * sinRollOver2;
	result.w = sinYawOver2 * cosPitchOver2 * cosRollOver2 - cosYawOver2 * sinPitchOver2 * sinRollOver2;
	return result;
}

Quaternion Quaternion::FromToRotation(
    const Vec3 fromDirection,
    const Vec3 toDirection)
{
	const float cosTheta = fromDirection * toDirection;
	const float k = sqrt(fromDirection.GetMagnitude() * toDirection.GetMagnitude());

	if (cosTheta / k == -1)
	{
		// 180 degree rotation around any orthogonal vector
		const auto res = fromDirection.GetOrthogonal().Normalize();
		return Quaternion(res, 0);
	}
	auto quat = Quaternion(Vec3::Cross(fromDirection, toDirection), cosTheta + k);
	quat.Normalize();
	return quat;
}

Quaternion Quaternion::RotateTowards(
    const Quaternion from,
    const Quaternion to,
    const float maxDegreesDelta)
{
    const float num = Angle(from, to);
    if (num == 0.0f) { return to; }
    const float t = std::min(1.0f, maxDegreesDelta / num);
    return SlerpUnclamped(from, to, t);
}

float Quaternion::Angle(const Quaternion a, const Quaternion b)
{
	const float f = Dot(a, b);
	return acos(std::min(abs(f), 1.0f)) * 2.0f * kRad2Deg;
}

Quaternion Quaternion::LookRotation(const Vec3 forward)
{
    const Vec3 up = {0, 1, 0};
    return LookRotation(forward, up);
}

//-----------------------------------------------------------------------------
// Operators
//-----------------------------------------------------------------------------
Quaternion Quaternion::operator+(const Quaternion& rhs) const
{
    return Quaternion(
        x + rhs.x,
        y + rhs.y,
        z + rhs.z,
        w + rhs.w);
}

Quaternion Quaternion::operator-(const Quaternion& rhs) const
{
    return Quaternion(
        x - x,
        y - y,
        z - z,
        w - w);
}

Quaternion Quaternion::operator*(const Quaternion& rhs) const
{
    return Quaternion(
        w * rhs.x +
        x * rhs.w +
        y * rhs.z -
        z * rhs.y,
        w * rhs.y +
        y * rhs.w +
        z * rhs.x -
        x * rhs.z,
        w * rhs.z +
        z * rhs.w +
        x * rhs.y -
        y * rhs.x,
        w * rhs.w -
        x * rhs.x -
        y * rhs.y -
        z * rhs.z);
}

Quaternion Quaternion::operator/(const Quaternion& rhs) const
{
    return *this * rhs.GetInverse();
}

bool Quaternion::operator==(const Quaternion& rhs) const
{
    return x == rhs.x &&
           y == rhs.y &&
           z == rhs.z &&
           w == rhs.w;
}

bool Quaternion::operator!=(const Quaternion& rhs) const
{
    return !(*this == rhs);
}

Quaternion Quaternion::operator+(const float rhs) const
{
    return Quaternion(
        x + rhs,
        y + rhs,
        z + rhs,
        w + rhs);
}

void Quaternion::operator+=(const float rhs)
{
    x += rhs;
    y += rhs;
    z += rhs;
    w += rhs;
}

Quaternion Quaternion::operator-(const float rhs) const
{
    return Quaternion(
        x - rhs,
        y - rhs,
        z - rhs,
        w - rhs);
}

void Quaternion::operator-=(const float rhs)
{
    x -= rhs;
    y -= rhs;
    z -= rhs;
    w -= rhs;
}

Quaternion Quaternion::operator*(const float rhs) const
{
    return Quaternion(
        x * rhs,
        y * rhs,
        z * rhs,
        w * rhs);
}

void Quaternion::operator*=(const float rhs)
{
    x *= rhs;
    y *= rhs;
    z *= rhs;
    w *= rhs;
}

Quaternion Quaternion::operator/(const float rhs) const
{
    return Quaternion(
        x / rhs,
        y / rhs,
        z / rhs,
        w / rhs);
}

void Quaternion::operator/=(const float rhs)
{
    x /= rhs;
    y /= rhs;
    z /= rhs;
    w /= rhs;
}

Quaternion Quaternion::LookRotation(Vec3 forward, Vec3 up)
{
    forward = forward.Normalize();
    const Vec3 right = Vec3::Cross(up, forward).Normalize();
    up = Vec3::Cross(forward, right);
    const auto m00 = right.x;
    const auto m01 = right.y;
    const auto m02 = right.z;
    const auto m10 = up.x;
    const auto m11 = up.y;
    const auto m12 = up.z;
    const auto m20 = forward.x;
    const auto m21 = forward.y;
    const auto m22 = forward.z;


    const float num8 = (m00 + m11) + m22;
    Quaternion quaternion;
    if (num8 > 0.0f) {
        auto num = sqrt(num8 + 1.0f);
        quaternion.w = num * 0.5f;
        num = 0.5f / num;
        quaternion.x = (m12 - m21) * num;
        quaternion.y = (m20 - m02) * num;
        quaternion.z = (m01 - m10) * num;
        return quaternion;
    }
    if ((m00 >= m11) && (m00 >= m22)) {
        const auto num7 = sqrt(((1.0f + m00) - m11) - m22);
        const auto num4 = 0.5f / num7;
        quaternion.x = 0.5f * num7;
        quaternion.y = (m01 + m10) * num4;
        quaternion.z = (m02 + m20) * num4;
        quaternion.w = (m12 - m21) * num4;
        return quaternion;
    }
    if (m11 > m22) {
        const auto num6 = sqrt(((1.0f + m11) - m00) - m22);
        const auto num3 = 0.5f / num6;
        quaternion.x = (m10 + m01) * num3;
        quaternion.y = 0.5f * num6;
        quaternion.z = (m21 + m12) * num3;
        quaternion.w = (m20 - m02) * num3;
        return quaternion;
    }
    const auto num5 = sqrt(((1.0f + m22) - m00) - m11);
    const auto num2 = 0.5f / num5;
    quaternion.x = (m20 + m02) * num2;
    quaternion.y = (m21 + m12) * num2;
    quaternion.z = 0.5f * num5;
    quaternion.w = (m01 - m10) * num2;
    return quaternion;
}

Vec3 Quaternion::NormalizeAngles(Vec3 angles)
{
    angles.x = NormalizeAngle(angles.x);
    angles.y = NormalizeAngle(angles.y);
    angles.z = NormalizeAngle(angles.z);
    return angles;
}

float Quaternion::LengthSquared() const
{
    return x * x + y * y + z * z + w * w;
}

Vec3 Quaternion::ToEulerRad(const Quaternion rotation)
{
	/*
	reference
	http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
	*/

	Vec3 eulerAngles;

	// Threshold for the singularities found at the north/south poles.
	const float SINGULARITY_THRESHOLD = 0.4999995f;

	auto sqw = rotation.w * rotation.w;
	auto sqx = rotation.x * rotation.x;
	auto sqy = rotation.y * rotation.y;
	auto sqz = rotation.z * rotation.z;
	auto unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
	auto singularityTest = (rotation.x * rotation.z) + (rotation.w * rotation.y);

	if (singularityTest > SINGULARITY_THRESHOLD * unit)
	{
		eulerAngles.z = (float)(2 * atan2(rotation.x, rotation.w));
		eulerAngles.y = kPi / 2.0f;
		eulerAngles.x = 0;
	}
	else if (singularityTest < -SINGULARITY_THRESHOLD * unit)
	{
		eulerAngles.z = (float)(-2 * atan2(rotation.x, rotation.w));
		eulerAngles.y = -kPi / 2.0f;
		eulerAngles.x = 0;
	}
	else
	{
		eulerAngles.z = (float)atan2(2 * ((rotation.w * rotation.z) - (rotation.x * rotation.y)), sqw + sqx - sqy - sqz);
		eulerAngles.y = (float)asin(2 * singularityTest / unit);
		eulerAngles.x = (float)atan2(2 * ((rotation.w * rotation.x) - (rotation.y * rotation.z)), sqw - sqx - sqy + sqz);
	}

	return eulerAngles;
}

float Quaternion::NormalizeAngle(float angle)
{
    while (angle > 360)
        angle -= 360;
    while (angle < 0)
        angle += 360;
    return angle;
}

Quaternion Quaternion::SlerpUnclamped(const Quaternion a, Quaternion b, float t)
{
    // if either input is zero, return the other.
    if (a.LengthSquared() == 0.0f) {
        if (b.LengthSquared() == 0.0f) {
            return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
        }
        return b;
    } else if (b.LengthSquared() == 0.0f) {
        return a;
    }


    float cosHalfAngle = (a.w * b.w) + (Vec3(a.x, a.y, a.z) * Vec3(b.x, b.y, b.z));

    if (cosHalfAngle >= 1.0f || cosHalfAngle <= -1.0f) {
        return a;
    } else if (cosHalfAngle < 0.0f) {
        b.x = -b.x;
        b.y = -b.y;
        b.z = -b.z;
        b.w = -b.w;
        cosHalfAngle = -cosHalfAngle;
    }

    float blendA;
    float blendB;
    if (cosHalfAngle < 0.99f) {
        const float halfAngle = acos(cosHalfAngle);
        const float sinHalfAngle = sin(halfAngle);
        const float oneOverSinHalfAngle = 1.0f / sinHalfAngle;
        blendA = sin(halfAngle * (1.0f - t)) * oneOverSinHalfAngle;
        blendB = sin(halfAngle * t) * oneOverSinHalfAngle;
    } else {
        blendA = 1.0f - t;
        blendB = t;
    }

    auto result = Quaternion(
        Vec3(a.x, a.y, a.z) * blendA + Vec3(b.x, b.y, b.z) * blendB,
        blendA * a.w + blendB * b.w);
    if (result.LengthSquared() > 0.0f) { return Normalize(result); } else {
        return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
    }
}
}
