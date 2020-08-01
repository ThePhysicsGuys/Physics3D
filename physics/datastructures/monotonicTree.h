#pragma once

#include <utility>
#include <cstddef>
#include <assert.h>

#include "unmanagedArray.h"

template<typename T>
struct MonotonicTreeNode {
	MonotonicTreeNode* children;
	T value;
};
template<typename T>
class MonotonicTree;

/*
	Warning: This is an unmanaged object, does not automatically free memory on delete
*/
template<typename T>
class MonotonicTreeBuilder {
	friend class MonotonicTree<T>;
	UnmanagedArray<MonotonicTreeNode<T>> allocatedMemory;
	MonotonicTreeNode<T>* currentAlloc;
public:

	MonotonicTreeBuilder() : allocatedMemory(), currentAlloc(nullptr) {}
	MonotonicTreeBuilder(UnmanagedArray<MonotonicTreeNode<T>>&& memory) : allocatedMemory(std::move(memory)), currentAlloc(allocatedMemory.get()) {}

	MonotonicTreeNode<T>* alloc(std::size_t size) {
		MonotonicTreeNode<T>* claimedBlock = currentAlloc;
		currentAlloc += size;
		assert(currentAlloc <= allocatedMemory.getEnd());
		return claimedBlock;
	}

	T* getPtrToFree() {
		return allocatedMemory.getPtrToFree();
	}
};

/*
	Warning: This is an unmanaged object, does not automatically free memory on delete
*/
template<typename T>
class MonotonicTree {
	UnmanagedArray<MonotonicTreeNode<T>> allocatedMemory;
public:

	MonotonicTree() : allocatedMemory() {}
	MonotonicTree(MonotonicTreeBuilder<T>&& builder) : allocatedMemory(std::move(builder.allocatedMemory)) {
		assert(builder.currentAlloc == this->allocatedMemory.getEnd());
	}
	MonotonicTree& operator=(MonotonicTreeBuilder<T>&& builder) {
		assert(builder.currentAlloc == builder.allocatedMemory.getEnd());

		this->allocatedMemory = std::move(builder.allocatedMemory);
	}
	
	MonotonicTreeNode<T>& operator[](std::size_t index) { return allocatedMemory[index]; }
	const MonotonicTreeNode<T>& operator[](std::size_t index) const {return allocatedMemory[index]; }

	MonotonicTreeNode<T>* begin() { return allocatedMemory.begin(); }
	const MonotonicTreeNode<T>* begin() const { return allocatedMemory.begin(); }
	MonotonicTreeNode<T>* end() { return allocatedMemory.end(); }
	const MonotonicTreeNode<T>* end() const { return allocatedMemory.end(); }

	MonotonicTreeNode<T>* getPtrToFree() {
		return allocatedMemory.getPtrToFree();
	}
};
