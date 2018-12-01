#include "mat4.h"

namespace Mat4Util {
	const Mat4 ZERO = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	const Mat4 IDENTITY = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
};

template<typename N>
CFrame Mat4Template<N>::toCFrame() const {
	return CFrame(getTranslation(), getRotation());
}
