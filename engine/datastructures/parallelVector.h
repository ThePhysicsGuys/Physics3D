#pragma once

#include "../math/vec3.h"

#include <immintrin.h>
// #include <cstdlib>


struct alignas(32) AlignedList {
	float v[8];

	float& operator[](int index) { return v[index]; }
	const float& operator[](int index) const { return v[index]; }
};

struct alignas(32) ParallelVec3 {
	union {
		__m256 xvalues;
		float x[8];
	};
	union {
		__m256 yvalues;
		float y[8];
	};
	union {
		__m256 zvalues;
		float z[8];
	};

	//AlignedList x, y, z;

	ParallelVec3() = default;
	inline ParallelVec3(Vec3f data[8]) {
		for(int i = 0; i < 8; i++) {
			x[i] = data[i].x;
			y[i] = data[i].y;
			z[i] = data[i].z;
		}
	}

	inline Vec3f operator[](unsigned int i) const {
		return Vec3f(x[i], y[i], z[i]);
	}

	inline __m256 dot(const Vec3f& vec) const {
		__m256 xt = _mm256_set1_ps(vec.x);
		__m256 yt = _mm256_set1_ps(vec.y);
		__m256 zt = _mm256_set1_ps(vec.z);
		
		__m256 xtt = _mm256_mul_ps(xt, xvalues);
		__m256 ytt = _mm256_mul_ps(yt, yvalues);
		__m256 ztt = _mm256_mul_ps(zt, zvalues);
		__m256 xy = _mm256_add_ps(xtt, ytt);
		return _mm256_add_ps(xy, ztt);
	}
};


struct AOSOAVec3fBuf {
	ParallelVec3* vecs;
	int size;

	AOSOAVec3fBuf(Vec3f* data, unsigned int size) : size(size) {
		unsigned int blockCount = (size + 7) / 8;
		// vecs = (ParallelVec3*) std::aligned_alloc(32, blockCount*sizeof(ParallelVec3),);
		vecs = new ParallelVec3[blockCount];

#ifdef CHECK_SANITY
		if (reinterpret_cast<size_t>(vecs) & 0b00011111 != 0) { throw "Error, did not align ParallelVec3 storage!"; }
#endif

		for(int i = 0; i < size / 8; i++) {
			for(int j = 0; j < 8; j++) {
				vecs[i].x[j] = data[i * 8 + j].x;
				vecs[i].y[j] = data[i * 8 + j].y;
				vecs[i].z[j] = data[i * 8 + j].z;
			}
		}
		if (size % 8 != 0) { // not perfectly aligned to block count
			int fillUntil = size & 7;
			for (int j = 0; j < fillUntil; j++) {
				vecs[size / 8].x[j] = data[size / 8 * 8 + j].x;
				vecs[size / 8].y[j] = data[size / 8 * 8 + j].y;
				vecs[size / 8].z[j] = data[size / 8 * 8 + j].z;
			}
			Vec3f lastVec = data[size / 8 * 8 + fillUntil - 1];
			for (int j = fillUntil; j < 8; j++) {
				vecs[size / 8].x[j] = lastVec.x;
				vecs[size / 8].y[j] = lastVec.y;
				vecs[size / 8].z[j] = lastVec.z;
			}
		}
	}

	AOSOAVec3fBuf(const AOSOAVec3fBuf& other) : size(other.size) {
		vecs = new ParallelVec3[(size + 7) / 8];
		/*for(int i = 0; i < (size + 7) / 8; i++) {
		vecs[i] = other.vecs[i];
		}*/
		this->vecs = other.vecs;
	}

	AOSOAVec3fBuf(AOSOAVec3fBuf&& other) : size(other.size), vecs(other.vecs) {
		other.vecs = nullptr;
	}

	AOSOAVec3fBuf& operator=(const AOSOAVec3fBuf& other) {
		size = other.size;
		/*vecs = new ParallelVec3[(size + 7) / 8];
		for(int i = 0; i < (size + 7) / 8; i++) {
		vecs[i] = other.vecs[i];
		}*/
		this->vecs = other.vecs;
		return *this;
	}

	AOSOAVec3fBuf& operator=(AOSOAVec3fBuf&& other) {
		size = other.size;
		vecs = other.vecs;
		other.vecs = nullptr;
		return *this;
	}

	~AOSOAVec3fBuf() {
		//delete[] vecs;
	}

	Vec3f operator[](unsigned int index) const { return vecs[index / 8][index & 7]; }

	const ParallelVec3* begin() const {return vecs;}
	ParallelVec3* begin() { return vecs; }
	const ParallelVec3* end() const {return vecs + blockCount();}
	ParallelVec3* end() { return vecs + blockCount(); }

	unsigned int blockCount() const { return (size + 7) / 8; }
};



struct ParallelVecIter {
	const AOSOAVec3fBuf& buf;
	int index;

	Vec3f operator*() const {
		return buf[index];
	}
	int operator++() {
		return ++index;
	}
	bool operator!=(const ParallelVecIter& other) const {
		return index != other.index;
	}
};

struct ParallelVecIterFactory {
	const AOSOAVec3fBuf& buf;
	int size;
	inline ParallelVecIter begin() const {
		return ParallelVecIter{buf, 0};
	};
	inline ParallelVecIter end() const {
		return ParallelVecIter{buf, size};
	};
};
