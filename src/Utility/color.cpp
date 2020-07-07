#include <Utility/color.h>
#include <CoreEngine/cassert.h>

namespace poke
{
const Color Color::clear("#000000", 0.0f);
const Color Color::black("#000000");
const Color Color::grey("#808080");
const Color Color::silver("#C0C0C0");
const Color Color::white("#FFFFFF");
const Color Color::maroon("#800000");
const Color Color::red("#FF0000");
const Color Color::olive("#808000");
const Color Color::yellow("#FFFF00");
const Color Color::orange("#FF8800");
const Color Color::green("#00FF00");
const Color Color::lime("#008000");
const Color Color::teal("#008080");
const Color Color::aqua("#00FFFF");
const Color Color::navy("#000080");
const Color Color::blue("#0000FF");
const Color Color::purple("#800080");
const Color Color::fuchsia("#FF00FF");

Color::Color(const float red, const float green, const float blue, const float alpha) :
	r(red),
	g(green),
	b(blue),
	a(alpha) {}

Color::Color(std::string hex, const float alpha) :
	a(alpha)
{
	if (hex[0] == '#') {
		hex.erase(0, 1);
	}

	cassert(hex.size() == 6, "Fack");
	const auto hexValue = std::stoul(hex, nullptr, 16);

	r = static_cast<float>((hexValue >> 16) & 0xff) / 255.0f;
	g = static_cast<float>((hexValue >> 8) & 0xff) / 255.0f;
	b = static_cast<float>((hexValue >> 0) & 0xff) / 255.0f;
}

bool Color::operator==(const Color& other) const
{
	return r == other.r && g == other.g && b == other.b && a == other.a;
}

bool Color::operator!=(const Color& other) const
{
	return !(*this == other);
}

float &Color::operator[](const uint32_t index)
{
	switch (index)
	{
	case 0:
		return r;
	case 1:
		return g;
	case 2:
		return b;
	case 3:
		return a;
	default:
		cassert(false, "Color is out of bound");
	}
}

std::ostream& operator<<(std::ostream& stream, const Color& color)
{
	return stream << color.r << ", " << color.g << ", " << color.b << ", " << color.a;
}
} //namespace poke
