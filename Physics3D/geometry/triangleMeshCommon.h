#pragma once

#include "triangleMesh.h"
#include <immintrin.h>
#include <stdint.h>

namespace P3D {
#ifdef _MSC_VER
#define GET_SSE_ELEM(reg, index) reg.m128_f32[index]
#define GET_SSE_ELEMi(reg, index) reg.m128i_i32[index]
#define GET_AVX_ELEM(reg, index) reg.m256_f32[index]
#else
#define GET_SSE_ELEM(reg, index) reg[index]
#define GET_AVX_ELEM(reg, index) reg[index]

inline static int mm_extractv_epi32(__m128i a, int b) {
	alignas(16) int buf[4];
	_mm_storeu_si128(reinterpret_cast<__m128i*>(buf), a);
	return buf[b];
}

#define GET_SSE_ELEMi(reg, index) mm_extractv_epi32(reg, index)
#endif

#define SWAP_2x2 0b01001110
#define SWAP_1x1 0b10110001

inline static size_t getOffset(size_t size) {
	return (size + 7) & 0xFFFFFFFFFFFFFFF8;
}

#ifdef _MSC_VER
inline static uint32_t countZeros(uint32_t mask) {
	unsigned long ret = 0;
	_BitScanForward(&ret, mask);
	return static_cast<int>(ret);
}
#else
inline static uint32_t countZeros(uint32_t mask) {
	return __builtin_ctz(mask);
}
#endif


static __m128 custom_fmadd_ps(__m128 a, __m128 b, __m128 c) {
	return  _mm_add_ps(_mm_mul_ps(a, b), c);
}
};
