#include "bufferLayout.h"

#include "GL\glew.h"
#include "../util/log.h"

namespace BufferDataType {
	const BufferDataTypeInfo NONE   = { "none" , 0, 0, 0 };
	const BufferDataTypeInfo BOOL   = { "bool" , GL_BYTE , 1, 1  };
	const BufferDataTypeInfo INT    = { "int"  , GL_INT  , 1, 4  };
	const BufferDataTypeInfo INT2   = { "vec2i", GL_INT  , 2, 8  };
	const BufferDataTypeInfo INT3   = { "vec3i", GL_INT  , 3, 12 };
	const BufferDataTypeInfo INT4   = { "vec4i", GL_INT  , 4, 16 };
	const BufferDataTypeInfo FLOAT  = { "float", GL_FLOAT, 1, 4  };
	const BufferDataTypeInfo FLOAT2 = { "vec2" , GL_FLOAT, 2, 8  };
	const BufferDataTypeInfo FLOAT3 = { "vec3" , GL_FLOAT, 3, 12 };
	const BufferDataTypeInfo FLOAT4 = { "vec4" , GL_FLOAT, 4, 16 };
	const BufferDataTypeInfo MAT2   = { "mat2" , GL_FLOAT, 2, 8  }; // per row
	const BufferDataTypeInfo MAT3   = { "mat3" , GL_FLOAT, 3, 12 }; // per row
	const BufferDataTypeInfo MAT4   = { "mat4" , GL_FLOAT, 4, 16 }; // per row
}

BufferLayout::BufferLayout() : stride(0) {};

BufferLayout::BufferLayout(std::vector<BufferElement> elements) : elements(elements), stride(0) {
	stride = 0;

	for (size_t i = 0; i < elements.size(); i++) {
		auto& element = elements[i];

		int multiplier = (element.info == BufferDataType::MAT2 || element.info == BufferDataType::MAT3 || element.info == BufferDataType::MAT4) ? element.info.count : 1;

		stride += element.info.size * multiplier;
	}
}

BufferLayout::~BufferLayout() {
	//Log::debug("Deleted buffer layout");
}