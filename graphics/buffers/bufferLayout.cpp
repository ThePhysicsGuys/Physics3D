#include "core.h"

#include "bufferLayout.h"

#include <GL/glew.h>

namespace P3D::Graphics {

namespace BufferDataType {

const Info NONE   = { "none"  , 0				, 0, 0 };
const Info BOOL   = { "bool"  , GL_BYTE			, 1, 1 };
const Info INT    = { "int"   , GL_INT			, 1, 4 };
const Info UINT   = { "uint"  , GL_UNSIGNED_INT , 1, 4 };
const Info INT2   = { "vec2i" , GL_INT			, 2, 8 };
const Info INT3   = { "vec3i" , GL_INT			, 3, 12 };
const Info INT4   = { "vec4i" , GL_INT			, 4, 16 };
const Info FLOAT  = { "float" , GL_FLOAT			, 1, 4 };
const Info FLOAT2 = { "vec2"  , GL_FLOAT			, 2, 8 };
const Info FLOAT3 = { "vec3"  , GL_FLOAT			, 3, 12 };
const Info FLOAT4 = { "vec4"  , GL_FLOAT			, 4, 16 };
const Info MAT2   = { "mat2"  , GL_FLOAT			, 2, 8 }; // per row
const Info MAT3   = { "mat3"  , GL_FLOAT			, 3, 12 }; // per row
const Info MAT4   = { "mat4"  , GL_FLOAT			, 4, 16 }; // per row

}

};