#include "core.h"

#include "color.h"

namespace P3D::Graphics {

namespace COLOR {

Color DISABLED = get(0xA0A0A0);
Color ACCENT = get(0x1F6678);
Color BACK = get(0x4D4D4D);
Color ALPHA = get(0x0, true);

Color RGB_R = get(0xFF0000);
Color RGB_G = get(0x00FF00);
Color RGB_B = get(0x0000FF);
Color A = get(0x0, true);

Color NAVY = get(0x001F3F);
Color BLUE = get(0x0074D9);
Color AQUA = get(0x7FDBFF);
Color TEAL = get(0x39CCCC);
Color OLIVE = get(0x3D9970);
Color GREEN = get(0x2ECC40);
Color LIME = get(0x01FF70);
Color YELLOW = get(0xFFDC00);
Color ORANGE = get(0xFF851B);
Color RED = get(0xFF4136);
Color MAROON = get(0x85144b);
Color FUCHSIA = get(0xF012BE);
Color PURPLE = get(0xB10DC9);
Color BLACK = get(0x111111);
Color GRAY = get(0xAAAAAA);
Color SILVER = get(0xDDDDDD);
Color WHITE = get(0xFFFFFF);

Color3 hsvToRgb(Color3 hsv) {
	float h = hsv.x * 360.0f;
	float s = hsv.y;
	float v = hsv.z;

	if (s == 0.0)
		return Color3(v, v, v);

	int hi = (int) (h / 60.0f) % 6;
	float f = (h / 60.0f) - hi;
	float p = v * (1.0f - s);
	float q = v * (1.0f - s * f);
	float t = v * (1.0f - s * (1.0f - f));

	switch (hi) {
		case 0:
			return Color3(v, t, p);
			break;
		case 1:
			return Color3(q, v, p);
			break;
		case 2:
			return Color3(p, v, t);
			break;
		case 3:
			return Color3(p, q, v);
			break;
		case 4:
			return Color3(t, p, v);
			break;
		case 5:
			return Color3(v, p, q);
			break;
	}

	return Color3();
}

Color hsvaToRgba(Color hsva) {
	Color3 color = hsvToRgb(Color3(hsva.x, hsva.y, hsva.z));
	return Color(color.x, color.y, color.z, hsva.w);
}

Color rgbaToHsva(Color rgba) {
	Color3 color = rgbToHsv(Color3(rgba.x, rgba.y, rgba.z));
	return Color(color.x, color.y, color.z, rgba.w);
}

Color3 rgbToHsv(Color3 rgb) {
	float r = rgb.x;
	float g = rgb.y;
	float b = rgb.z;

	float h = 0.0f;
	float s = 0.0f;
	float v = 0.0f;

	float min = fmin(fmin(r, g), b);
	float max = fmax(fmax(r, g), b);

	float d = max - min;
	v = max;

	if (d == 0.0f) {
		h = 0.0f;
		s = 0.0f;
	} else {
		s = d / max;

		float dr = (((max - r) / 6.0f) + (max / 2.0f)) / d;
		float dg = (((max - g) / 6.0f) + (max / 2.0f)) / d;
		float db = (((max - b) / 6.0f) + (max / 2.0f)) / d;

		if (r == max)
			h = db - dg;
		else if (g == max)
			h = 1.0f / 3.0f + dr - db;
		else if (b == max)
			h = 2.0f / 3.0f + dg - dr;

		if (h < 0.0f)
			h += 1.0f;
		if (h > 1.0f)
			h -= 1.0f;

	}

	return Color3(h, s, v);
}

Color blend(const Color& color1, const Color& color2, float blend) {
	return (1.0f - blend) * color1 + blend * color2;
}

Color3 blend(const Color3& color1, const Color3& color2, float blend) {
	return (1.0f - blend) * color1 + blend * color2;
}

};

};