#pragma once

#include "math/vec3.h"

void logVec(Vec3 origin, Vec3 vec);

void setVecLogAction(void(*logger)(Vec3 origin, Vec3 vec));
