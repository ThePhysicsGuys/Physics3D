#pragma once

#include "../../util/log.h"
#include "vec3.h"
#include "position.h"
#include "cframe.h"
#include "globalCFrame.h"

inline Position castVecToPos(Vec3 v, const char* file, int line) {
	//Log::warn("%s:%d used temp Vector to Position cast!", file, line);
	return Position(Fix<32>(v.x), Fix<32>(v.y), Fix<32>(v.z));
}
inline Vec3 castPosToVec(Position p, const char* file, int line) {
	//Log::warn("%s:%d used temp Position to Vector cast!", file, line);
	return Vec3(double(p.x), double(p.y), double(p.z));
}
inline CFrame castGlobalCFrameToCFrame(const GlobalCFrame& cf, const char* file, int line) {
	//Log::warn("%s:%d used temp GlobalCFrame to CFrame cast!", file, line);
	return CFrame(Vec3(double(cf.position.x), double(cf.position.y), double(cf.position.z)) , cf.rotation);
}
inline GlobalCFrame castCFrameToGlobalCFrame(const CFrame& cf, const char* file, int line) {
	//Log::warn("%s:%d used temp CFrame to GlobalCFrame cast!", file, line);
	return GlobalCFrame(Position(Fix<32>(cf.position.x), Fix<32>(cf.position.y), Fix<32>(cf.position.z)), cf.rotation);
}

#define TEMP_CAST_VEC_TO_POSITION(v) castVecToPos(v, __FILE__, __LINE__)
#define TEMP_CAST_POSITION_TO_VEC(p) castPosToVec(p, __FILE__, __LINE__)
#define TEMP_CAST_GLOBALCFRAME_TO_CFRAME(c) castGlobalCFrameToCFrame(c, __FILE__, __LINE__)
#define TEMP_CAST_CFRAME_TO_GLOBALCFRAME(c) castCFrameToGlobalCFrame(c, __FILE__, __LINE__)
