#include "part.h"

Part::Part() {}

Part::Part(const Shape& s, double density, double friction) : hitbox(s), properties({density, friction}) {}
