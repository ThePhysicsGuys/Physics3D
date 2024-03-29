#pragma once

#include <optional>

namespace P3D::Graphics {
	
// <type> <name> [(<uniform>)] [= <default> [(<start> : <stop>)]];
// vec3 TheInputColor (inputColor) = 0.1 (0:1), 0.2 (-10,10), 0.3;

struct Property {
	
	struct Range {
		float min;
		float max;
	};
	
	struct Default {
		float value = 0.0;
		std::optional<Range> range;
		std::optional<float> step;
	};

	enum class Type {
		None,
		Bool,
		Int,
		Float,
		Vec2,
		Vec3,
		Vec4,
		Mat2,
		Mat3,
		Mat4,
		Col3,
		Col4,
	};

	std::string_view name;
	std::string_view uniform;
	Type type = Type::None;
	std::vector<Default> defaults;
};
	
class PropertiesParser {
public:
	static std::vector<Property> parse(const char* source, std::string_view view);
};

}
