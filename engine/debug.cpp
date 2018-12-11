#include "debug.h"


namespace Debug {
	void(*logVecAction)(Vec3, Vec3, VecType) = [](Vec3, Vec3, VecType) {};

	void logVec(Vec3 origin, Vec3 vec, VecType type) { logVecAction(origin, vec, type); };

	void setVecLogAction(void(*logger)(Vec3 origin, Vec3 vec, VecType type)) { logVecAction = logger; };
}
