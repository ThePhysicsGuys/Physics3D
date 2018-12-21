#include "debug.h"


namespace Debug {
	void(*logVecAction)(Vec3, Vec3, VecType) = [](Vec3, Vec3, VecType) {};
	void(*logCFrameAction)(CFrame, CFrameType) = [](CFrame, CFrameType) {};

	void logVec(Vec3 origin, Vec3 vec, VecType type) { logVecAction(origin, vec, type); };
	void logCFrame(CFrame frame, CFrameType type) { logCFrameAction(frame, type); };
	void setVecLogAction(void(*logger)(Vec3 origin, Vec3 vec, VecType type)) { logVecAction = logger; };
	void setCFrameLogAction(void(*logger)(CFrame frame, CFrameType type)) { logCFrameAction = logger; };
}
