#pragma once

#include <utility>
#include <cstddef>
#include <assert.h>

template<typename T>
struct MonotonicTreeNode {
	MonotonicTreeNode* children;
	T value;
};
template<typename T>
class MonotonicTree;
template<typename T>
class MonotonicTreeBuilder {
	friend class MonotonicTree<T>;
	std::unique_ptr<MonotonicTreeNode<T>[]> allocatedMemory;
	MonotonicTreeNode<T>* currentAlloc;
	MonotonicTreeNode<T>* endOfAlloc;
public:

	MonotonicTreeBuilder() : allocatedMemory(nullptr), currentAlloc(nullptr), endOfAlloc(nullptr) {}
	MonotonicTreeBuilder(std::size_t treeSize) : allocatedMemory(std::make_unique<MonotonicTreeNode<T>[]>(treeSize)), currentAlloc(allocatedMemory.get()+1), endOfAlloc(allocatedMemory.get() + treeSize) {}

	MonotonicTreeNode<T>& getRootNode() { return *allocatedMemory.get(); }
	const MonotonicTreeNode<T>& getRootNode() const { return *allocatedMemory.get(); }

	MonotonicTreeNode<T>* alloc(std::size_t size) {
		MonotonicTreeNode<T>* claimedBlock = currentAlloc;
		currentAlloc += size;
		assert(currentAlloc <= endOfAlloc);
		return claimedBlock;
	}
};
template<typename T>
class MonotonicTree {
	std::unique_ptr<MonotonicTreeNode<T>[]> allocatedMemory;
	MonotonicTreeNode<T>* endOfAlloc;
public:

	MonotonicTree() : allocatedMemory(nullptr) {}
	MonotonicTree(MonotonicTreeBuilder<T>&& builder) : allocatedMemory(std::move(builder.allocatedMemory)), endOfAlloc(builder.endOfAlloc) {
		assert(builder.currentAlloc == builder.endOfAlloc);
		builder.endOfAlloc = nullptr;
		builder.currentAlloc = nullptr;
	}
	MonotonicTree& operator=(MonotonicTreeBuilder<T>&& builder) {
		this->allocatedMemory = std::move(builder.allocatedMemory);
		this->endOfAlloc = builder.endOfAlloc;

		assert(builder.currentAlloc == builder.endOfAlloc);

		builder.allocatedMemory = nullptr;
		builder.endOfAlloc = nullptr;
	}
	
	MonotonicTreeNode<T>& getRootNode() {return *allocatedMemory.get();}
	const MonotonicTreeNode<T>& getRootNode() const {return *allocatedMemory.get();}

	MonotonicTreeNode<T>& operator[](std::size_t index) { return allocatedMemory[index]; }
	const MonotonicTreeNode<T>& operator[](std::size_t index) const {return allocatedMemory[index]; }

	MonotonicTreeNode<T>* begin() { return allocatedMemory.get(); }
	const MonotonicTreeNode<T>* begin() const { return allocatedMemory.get(); }
	MonotonicTreeNode<T>* end() { return endOfAlloc; }
	const MonotonicTreeNode<T>* end() const { return endOfAlloc; }
};
