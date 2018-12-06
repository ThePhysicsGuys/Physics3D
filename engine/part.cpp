#include "part.h"

Part::Part() {}

Part::Part(Shape s, double density, double friction) : hitbox(s), properties({density, friction}) {}
