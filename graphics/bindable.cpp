#include "core.h"

#include "bindable.h"

namespace Graphics {

Bindable::Bindable() : id(0) {}

Bindable::Bindable(GLID id) : id(id) {}

GLID Bindable::getID() const {
	return id;
}

};