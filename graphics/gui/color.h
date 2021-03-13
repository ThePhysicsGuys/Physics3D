#pragma once

typedef Vec4f Color;
typedef Vec3f Color3;

namespace P3D::Graphics {

// Colors
namespace COLOR {
	extern Color DISABLED;
	extern Color ACCENT;
	extern Color BACK;
	extern Color ALPHA;

	extern Color RGB_R;
	extern Color RGB_G;
	extern Color RGB_B;
	extern Color A;

	extern Color NAVY;
	extern Color BLUE;
	extern Color AQUA;
	extern Color TEAL;
	extern Color OLIVE;
	extern Color GREEN;
	extern Color LIME;
	extern Color YELLOW;
	extern Color ORANGE;
	extern Color RED;
	extern Color MAROON;
	extern Color FUCHSIA;
	extern Color PURPLE;
	extern Color BLACK;
	extern Color GRAY;
	extern Color SILVER;
	extern Color WHITE;

	// format: 0xRRGGBBAA
	constexpr Color get(int hex, bool alpha) {
		if (!alpha) 
			hex = hex << 8 | 0xFF;
		
		Color color;
		color.x = static_cast<float>(hex >> 24 & 0xFF) / 255.0f;
		color.y = static_cast<float>(hex >> 16 & 0xFF) / 255.0f;
		color.z = static_cast<float>(hex >> 8 & 0xFF) / 255.0f;
		color.w = static_cast<float>(hex & 0xFF) / 255.0f;
		
		return color;
	}

	constexpr Color get(int hex) {
		return get(hex, false);
	}

	Color3 hsvToRgb(const Color3& hsv);
	Color3 rgbToHsv(const Color3& rgb);
	Color hsvaToRgba(const Color& hsva);
	Color rgbaToHsva(const Color& rgba);

	Color blend(const Color& color1, const Color& color2, float blend);
	Color3 blend(const Color3& color1, const Color3& color2, float blend);
};

};