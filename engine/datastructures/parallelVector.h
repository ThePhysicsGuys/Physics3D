#pragma once

#include "../math/vec3.h"
#include "../../util/log.h"

struct alignas(32) ParallelVec3 {
	float xvalues[8];
	float yvalues[8];
	float zvalues[8];

	ParallelVec3() = default;
	inline ParallelVec3(const Vec3f data[8]) {
		this->load(data);
	}

	inline void load(const Vec3f data[8]) {
		for (int i = 0; i < 8; i++) {
			xvalues[i] = data[i].x;
			yvalues[i] = data[i].y;
			zvalues[i] = data[i].z;
		}
	}

	inline Vec3f operator[](unsigned int i) const {
		return Vec3f(xvalues[i], yvalues[i], zvalues[i]);
	}
};

inline ParallelVec3* createParallelVecBuf(size_t blockCount) {
	ParallelVec3* vecs = new ParallelVec3[blockCount];
	// TODO fix alignment bug
	if (reinterpret_cast<size_t>(vecs) & 0b00011111 != 0) {
		//delete[] vecs;
		//throw "Error, did not align ParallelVec3 storage!";
		Log::error("VecBuf Not aligned!");
	}
	return vecs;
}

inline ParallelVec3* createAndFillParallelVecBuf(const Vec3f* data, size_t size) {
	size_t blockCount = (size+7) / 8;
	ParallelVec3* vecs = createParallelVecBuf(blockCount);

	unsigned int alignedSize = size & 0xFFFFFFF8;

	// fill all the easly fillable blocks with their data
	for (int i = 0; i < size / 8; i++) {
		vecs[i].load(data + i * 8);
	}

	// fill the remainder of the last block with the last vector, just to have filler data there
	// this makes it that for example getFurthestInDirection does not have to include a special case for the last block of vecs
	size_t remaining = size & 7;
	if (remaining != 0) { // not perfectly aligned to block count
		Vec3f lastBlock[8];
		Vec3f lastVec = data[alignedSize + remaining - 1];
		size_t lastBlockIndex = size / 8;
		for (size_t i = 0; i < remaining; i++)
			lastBlock[i] = data[alignedSize + i];

		for (size_t i = remaining; i < 8; i++)
			lastBlock[i] = lastVec;

		vecs[lastBlockIndex].load(lastBlock);
	}
	return vecs;
}
/*
struct AOSOAVec3fBuf {
	ParallelVec3* vecs;
	int size;

	inline AOSOAVec3fBuf() : vecs(nullptr), size(0) {}

	inline AOSOAVec3fBuf(const Vec3f* data, unsigned int size) : size(size) {
		unsigned int blockCount = (size + 7) / 8;
		vecs = createParallelVecBuf(blockCount);

		unsigned int alignedSize = size & 0xFFFFFFF8;

		// fill all the easly fillable blocks with their data
		for(int i = 0; i < size / 8; i++) {
			vecs[i].load(data + i * 8);
		}

		// fill the remainder of the last block with the last vector, just to have filler data there
		// this makes it that for example getFurthestInDirection does not have to include a special case for the last block of vecs
		unsigned int remaining = size & 7;
		if (remaining != 0) { // not perfectly aligned to block count
			Vec3f lastBlock[8];
			Vec3f lastVec = data[alignedSize + remaining - 1];
			unsigned int lastBlockIndex = size / 8;
			for (int i = 0; i < remaining; i++)
				lastBlock[i] = data[alignedSize + i];

			for (int i = remaining; i < 8; i++)
				lastBlock[i] = lastVec;
			
			vecs[lastBlockIndex].load(lastBlock);
		}
	}

	inline AOSOAVec3fBuf(const AOSOAVec3fBuf& other) : size(other.size) {
		vecs = createParallelVecBuf(other.blockCount());
		for (unsigned int i = 0; i < other.blockCount(); i++) {
			vecs[i] = other.vecs[i];
		}
	}

	inline AOSOAVec3fBuf& operator=(const AOSOAVec3fBuf& other) {
		delete[] this->vecs;
		vecs = createParallelVecBuf(other.blockCount());
		for (unsigned int i = 0; i < other.blockCount(); i++) {
			vecs[i] = other.vecs[i];
		}
		this->size = other.size;
	}

	inline AOSOAVec3fBuf(AOSOAVec3fBuf&& other) : size(other.size), vecs(other.vecs) {
		other.vecs = nullptr;
		other.size = 0;
	}

	inline AOSOAVec3fBuf& operator=(AOSOAVec3fBuf&& other) {
		std::swap(this->vecs, other.vecs);
		std::swap(this->size, other.size);
		return *this;
	}

	inline ~AOSOAVec3fBuf() {
		delete[] vecs;
	}

	inline Vec3f operator[](unsigned int index) const { return vecs[index / 8][index & 7]; }

	inline const ParallelVec3* begin() const {return vecs;}
	inline ParallelVec3* begin() { return vecs; }
	inline const ParallelVec3* end() const {return vecs + blockCount();}
	inline ParallelVec3* end() { return vecs + blockCount(); }

	inline unsigned int blockCount() const { return (size + 7) / 8; }
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
*/