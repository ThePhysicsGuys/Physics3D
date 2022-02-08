#pragma once

#include "components.h"

namespace P3D::Graphics {
	namespace Materials {
		inline Comp::Material metal(const Color& color) {
			return Comp::Material(color, 1.0, 0.2, 1.0);
		}

		inline Comp::Material metalRough(const Color& color) {
			return Comp::Material(color, 1.0, 0.65, 1.0);
		}

		inline Comp::Material metalDiffuse(const Color& color) {
			return Comp::Material(color, 0.75, 0.35, 1.0);
		}

		inline Comp::Material metalNoReflection(const Color& color) {
			return Comp::Material(color, 1.0, 0.0, 1.0);
		}

		inline Comp::Material plastic(const Color& color) {
			return Comp::Material(color, 0.0, 0.35, 1.0);
		}

		inline Comp::Material plasticDiffuse(const Color& color) {
			return Comp::Material(color, 0.0, 0.8, 0.8);
		}

		inline Comp::Material plasticDark(const Color& color) {
			return Comp::Material(color, 0.0, 0.0, 0.0);
		}

		inline Comp::Material plasticNoReflection(const Color& color) {
			return Comp::Material(color, 0.0, 0.0, 1.0);
		}

		inline static Comp::Material COPPER = metal(Color::get<0xB24819>());
		inline static Comp::Material GOLD = metal(Color::get<0xFFDE00>());
		inline static Comp::Material SILVER = metal(Color::get<0xD9D9D9>());
		inline static Comp::Material IRON = metalDiffuse(Color::get<0xAAAAAA>());
		inline static Comp::Material WHITE = plastic(Color::get<0xFFFFFF>());
		inline static Comp::Material RED = plasticDiffuse(Color::get<0xFF0000>());
		inline static Comp::Material GREEN = plasticDiffuse(Color::get<0x00FF00>());
		inline static Comp::Material BLUE = plasticDiffuse(Color::get<0x0000FF>());
	}
};