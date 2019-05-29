#pragma once

template<typename T>
class LargeMatrix {
	T* data;
public:
	size_t width, height;

	inline LargeMatrix(size_t N, size_t M) : width(N), height(M), data(new T[N*M]) {}
	~LargeMatrix() { delete[] data; }

	inline LargeMatrix(const LargeMatrix& other) : width(other.width), height(other.height), data(new T[width * height]) {
		for (int i = 0; i < width * height; i++) {
			this->data[i] = other.data[i];
		}
	}
	inline LargeMatrix& operator=(const LargeMatrix& other) {
		delete[] data;
		this->height = other.height;
		this->width = other.width;
		data = new T[height * width];
		for (int i = 0; i < width * height; i++) {
			this->data[i] = other.data[i];
		}
		return *this;
	}

	inline LargeMatrix(LargeMatrix&& other) {
		T* tmpData = data;
		data = other.data;
		other.data = tmpData;

		size_t tmpHeight = height;
		height = other.height;
		other.height = tmpHeight;

		size_t tmpWidth = width;
		width = other.width;
		other.width = tmpWidth;
	}

	inline LargeMatrix& operator=(LargeMatrix&& other) {
		T* tmpData = data;
		data = other.data;
		other.data = tmpData;

		size_t tmpHeight = height;
		height = other.height;
		other.height = tmpHeight;

		size_t tmpWidth = width;
		width = other.width;
		other.width = tmpWidth;
		return *this;
	}

	T* operator[](size_t index) {
		return data + width * index;
	}
	const T* operator[](size_t index) const {
		return data + width * index;
	}

	const T* begin() const { return data; }
	const T* end() const { return data + width * height; }
	T* begin() { return data; }
	T* end() { return data + width * height; }
};
