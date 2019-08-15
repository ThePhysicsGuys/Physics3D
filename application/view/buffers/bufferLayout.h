#pragma once

#include <vector>
#include <string>

namespace BufferDataType {

	struct BufferDataTypeInfo {
		// name of data type
		std::string name;

		// data type of each component
		int type;  

		// amount of components per attributes
		int count; 

		// size of type in bytes
		int size;  

		BufferDataTypeInfo(std::string name, int type, int count, int size) : name(name), type(type), count(count), size(size) {};

		bool operator==(const BufferDataTypeInfo& other) const {
			return other.name == name;
		}
	};

	extern const BufferDataTypeInfo NONE;
	extern const BufferDataTypeInfo BOOL;
	extern const BufferDataTypeInfo INT;
	extern const BufferDataTypeInfo INT2;
	extern const BufferDataTypeInfo INT3;
	extern const BufferDataTypeInfo INT4;
	extern const BufferDataTypeInfo FLOAT;
	extern const BufferDataTypeInfo FLOAT2;
	extern const BufferDataTypeInfo FLOAT3;
	extern const BufferDataTypeInfo FLOAT4;
	extern const BufferDataTypeInfo MAT2;
	extern const BufferDataTypeInfo MAT3;
	extern const BufferDataTypeInfo MAT4;
};

struct BufferElement {
	std::string name;
	BufferDataType::BufferDataTypeInfo info;
	bool normalized;

	BufferElement(std::string name, BufferDataType::BufferDataTypeInfo info, bool normalized = false);
};

struct BufferLayout {
	std::vector<BufferElement> elements;
	int stride;

	BufferLayout();
	BufferLayout(std::vector<BufferElement> elements);
	~BufferLayout();
};