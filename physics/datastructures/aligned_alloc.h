#pragma once

#include <cstddef>

namespace P3D {
void* aligned_malloc(std::size_t size, std::size_t align);
void aligned_free(void* ptr);
};
