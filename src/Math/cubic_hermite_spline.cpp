#include <Math/cubic_hermite_spline.h>

namespace poke {
namespace math {

CubicHermiteSpline::CubicHermiteSpline(const std::vector<Vec3>& points) {
	cassert(
		points.size() < maxSize,
		"CubiHermiteSpline::SetPoints => The size cannot be greater than 256.");

    for (const auto point : points) {
		points_[front_] = point;
		front_++;
    }
}

bool CubicHermiteSpline::operator==(const CubicHermiteSpline& other) const
{
    return front_ == other.front_ && points_ == other.points_;
}

bool CubicHermiteSpline::operator!=(const CubicHermiteSpline& other) const
{
    return !(*this == other);
}

void CubicHermiteSpline::AddPoint(const Vec3 point)
{
    points_[front_] = point;
    front_++;

	totalLength_ = 0;
	for (int i = 1; i < front_ - 2; i++) {
		totalLength_ += GetSegmentLength(i);
	}
}

Vec3 CubicHermiteSpline::Lerp(const int pointIndex, const double percent) const
{
    const auto p0 = points_[pointIndex - 1];
    const auto p1 = points_[pointIndex];
    const auto p2 = points_[pointIndex + 1];
    const auto p3 = points_[pointIndex + 2];

    const auto t0 = 0.0;
    const auto t1 = GetPortionOfDistance(t0, p0, p1);
    const auto t2 = GetPortionOfDistance(t1, p1, p2);
    const auto t3 = GetPortionOfDistance(t2, p0, p3);

    const auto t = ((t2 - t1) * percent) + t1;
    const auto a1 = p0 * (t1 - t) / (t1 - t0) + p1 * (t - t0) / (t1 - t0);
    const auto a2 = p1 * (t2 - t) / (t2 - t1) + p2 * (t - t1) / (t2 - t1);
    const auto a3 = p2 * (t3 - t) / (t3 - t2) + p3 * (t - t2) / (t3 - t2);

    const auto b1 = a1 * (t2 - t) / (t2 - t0) + a2 * (t - t0) / (t2 - t0);
    const auto b2 = a2 * (t3 - t) / (t3 - t1) + a3 * (t - t1) / (t3 - t1);

	const auto c = b1 * (t2 - t) / (t2 - t1) + b2 * (t - t1) / (t2 - t1);
    return c;
}

float CubicHermiteSpline::GetSegmentLength(const int pointIndex) const
{
    if (pointIndex < front_) {
        return Vec3::GetDistance(
            points_[pointIndex],
            points_[pointIndex + 1]);
    }
    return 0;
}

float CubicHermiteSpline::GetTotalLength() const
{
	return totalLength_;
}

std::vector<Vec3> CubicHermiteSpline::GetPoints() const
{
    std::vector<Vec3> points(front_);
    for (int i = 0; i < front_; i++) { points[i] = points_[i]; }
	return points;
}

void CubicHermiteSpline::SetPoint(const Vec3 point, const uint8_t pos)
{
    points_[pos] = point;

	totalLength_ = 0;
	for (int i = 1; i < front_ - 2; i++) {
		totalLength_ += GetSegmentLength(i);
	}
}

void CubicHermiteSpline::SetPoints(const std::vector<Vec3>& points)
{
    cassert(
		points.size() < maxSize,
        "CubiHermiteSpline::SetPoints => The size cannot be greater than 256.");

    for (int i = 0; i < points.size(); i++) { points_[i] = points[i]; }
	front_ = points.size();

	totalLength_ = 0;
	for (int i = 1; i < front_ - 2; i++) {
		totalLength_ += GetSegmentLength(i);
	}
}

int CubicHermiteSpline::GetSize() const { return front_; }
int CubicHermiteSpline::GetMaxSize() const { return maxSize; }

double CubicHermiteSpline::GetPortionOfDistance(const double t, const Vec3 p0, const Vec3 p1) const
{
    const auto a = pow(p1.x - p0.x, 2.0f) + pow(p1.y - p0.y, 2.0f) + pow(p1.z - p0.z, 2.0f);
	const auto b = pow(a, 0.5f);
	const auto c = pow(b, 1);

    return c + t;
}
} //namespace math
} //namespace poke
