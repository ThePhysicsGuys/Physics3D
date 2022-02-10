#pragma once

namespace P3D::Graphics {

#define DEFAULT_UNIFORM_BUFFER_LAYOUT \
	BufferLayout({ \
		BufferElement("vModelMatrix", BufferDataType::MAT4, true), \
		BufferElement("vAlbedo", BufferDataType::FLOAT4, true), \
		BufferElement("vMRAo", BufferDataType::FLOAT3, true), \
		BufferElement("vTextureFlags", BufferDataType::UINT2, true) \
	})

namespace BufferDataType {

	struct Info {
		// name of data type
		std::string name;

		// data type of each component
		int type;

		// amount of components per attributes
		int count;

		// size of type in bytes
		int size;

		// Whether the type is integral
		bool integral;

		Info(const std::string& name, int type, int count, int size, bool integral)
			: name(name)
			, type(type)
			, count(count)
			, size(size)
			, integral(integral) {}

		bool operator==(const Info& other) const {
			return other.name == name;
		}
	};

	extern const Info NONE;
	extern const Info BOOL;
	extern const Info UINT;
	extern const Info UINT2;
	extern const Info UINT3;
	extern const Info UINT4;
	extern const Info INT;
	extern const Info INT2;
	extern const Info INT3;
	extern const Info INT4;
	extern const Info FLOAT;
	extern const Info FLOAT2;
	extern const Info FLOAT3;
	extern const Info FLOAT4;
	extern const Info MAT2;
	extern const Info MAT3;
	extern const Info MAT4;

};

struct BufferElement {
	std::string name;
	BufferDataType::Info info;
	bool instanced;
	bool normalized;

	BufferElement(const std::string& name, const BufferDataType::Info& info, bool instanced = false, bool normalized = false)
		: name(name)
		, info(info)
		, instanced(instanced)
		, normalized(normalized) {}
};

struct BufferLayout {
	std::vector<BufferElement> elements;
	int stride;

	BufferLayout(const std::vector<BufferElement>& elements)
		: elements(elements)
		, stride(0) {
		for (BufferElement& element : this->elements) {
			int multiplier = (element.info == BufferDataType::MAT2 || element.info == BufferDataType::MAT3 || element.info == BufferDataType::MAT4)
				                 ? element.info.count
				                 : 1;
			stride += element.info.size * multiplier;
		}
	}

	BufferLayout()
		: elements({})
		, stride(0) {}
};

};
