#include "debug.h"

void(*logVecAction)(Vec3, Vec3) = [](Vec3, Vec3) {};

void logVec(Vec3 origin, Vec3 vec) { logVecAction(origin, vec); };

void setVecLogAction(void(*logger)(Vec3 origin, Vec3 vec)) { logVecAction = logger; };
