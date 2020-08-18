#include "core.h"

#include "bindable.h"

namespace P3D::Graphics {

Bindable::Bindable() : id(0) {}

Bindable::Bindable(GLID id) : id(id) {}

GLID Bindable::getID() const {
	return id;
}

};