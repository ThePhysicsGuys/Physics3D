#pragma once

#include "world.h"
#include "layer.h"

namespace P3D {
// expects a function of the form void(Part& part)
template<typename Func>
void WorldPrototype::forEachPart(const Func& funcToRun) const {
	for(const ColissionLayer& layer : this->layers) {
		layer.forEach(funcToRun);
	}
}
// expects a function of the form void(Part& part)
template<typename Func, typename Filter>
void WorldPrototype::forEachPartFiltered(const Filter& filter, const Func& funcToRun) const {
	for(const ColissionLayer& layer : this->layers) {
		layer.forEachFiltered(filter, funcToRun);
	}
}

// expects a function of the form void(T& part)
template<typename T>
template<typename Func>
void World<T>::forEachPart(const Func& funcToRun) const {
	static_cast<const WorldPrototype*>(this)->forEachPart([&funcToRun](Part& part) {funcToRun(static_cast<T&>(part)); });
}
// expects a function of the form void(T& part)
template<typename T>
template<typename Func, typename Filter>
void World<T>::forEachPartFiltered(const Filter& filter, const Func& funcToRun) const {
	static_cast<const WorldPrototype*>(this)->forEachPartFiltered(filter, [&funcToRun](Part& part) {funcToRun(static_cast<T&>(part)); });
}
};

