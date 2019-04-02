#pragma once

class Screen;
class Camera;
struct Physical;

#include "../engine/math/vec3.h"

typedef void (*CameraMoveHandler) (Screen&, Camera*, Vec3);
typedef void (*WindowResizeHandler) (Screen&, unsigned int, unsigned int);
typedef void (*PhysicalRayIntersectHandler) (Screen&, Physical*, Vec3);
typedef void (*PhysicalDragHandler) (Screen&, Physical*, Vec3);
typedef void (*PhysicalClickHandler) (Screen&, Physical*, Vec3);
typedef void (*PhysicalTouchHandler) (Physical*, Physical*, Vec3);
typedef void (*PhysicalReleaseHandler) (Physical*, Physical*);

class EventHandler {
private:
	void* pointer;

public:
	CameraMoveHandler cameraMoveHandler = [] (Screen&, Camera*, Vec3) {};
	WindowResizeHandler windowResizeHandler = [] (Screen&, unsigned int, unsigned int) {};
	PhysicalRayIntersectHandler physicalRayIntersectHandler = [] (Screen&, Physical*, Vec3) {};
	PhysicalDragHandler physicalDragHandler = [] (Screen&, Physical*, Vec3) {};
	PhysicalClickHandler physicalClickHandler = [] (Screen&, Physical*, Vec3) {};
	PhysicalTouchHandler physicalTouchHandler = [] (Physical*, Physical*, Vec3) {};
	PhysicalReleaseHandler physicalReleaseHandler = [] (Physical*, Physical*) {};
	
	void* getPtr() const;
	void setPtr(void* ptr);
	void setCameraMoveCallback(CameraMoveHandler handler);
	void setWindowResizeCallback(WindowResizeHandler handler);
	void setPhysicalRayIntersectCallback(PhysicalRayIntersectHandler handler);
	void setPhysicalDragCallback(PhysicalDragHandler handler);
	void setPhysicalClickCallback(PhysicalClickHandler handler);
	void setPhysicalTouchCallback(PhysicalTouchHandler handler);
	void setPhysicalReleaseCallback(PhysicalReleaseHandler handler);
};


