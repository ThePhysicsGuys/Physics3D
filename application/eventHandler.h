#pragma once

class Screen;
class Camera;

#include "../engine/part.h"
#include "../engine/math/vec3.h"

typedef void (*CameraMoveHandler) (Screen&, Camera*, Vec3);
typedef void (*WindowResizeHandler) (Screen&, unsigned int, unsigned int);
typedef void (*PartRayIntersectHandler) (Screen&, Part*, Vec3);
typedef void (*PartDragHandler) (Screen&, Part*, Vec3);
typedef void (*PartClickHandler) (Screen&, Part*, Vec3);
typedef void (*PartTouchHandler) (Part*, Part*, Vec3);
typedef void (*PartReleaseHandler) (Part*, Part*);

class EventHandler {
private:
	void* pointer;

public:
	CameraMoveHandler cameraMoveHandler = [] (Screen&, Camera*, Vec3) {};
	WindowResizeHandler windowResizeHandler = [] (Screen&, unsigned int, unsigned int) {};
	PartRayIntersectHandler partRayIntersectHandler = [] (Screen&, Part*, Vec3) {};
	PartDragHandler partDragHandler = [] (Screen&, Part*, Vec3) {};
	PartClickHandler partClickHandler = [] (Screen&, Part*, Vec3) {};
	PartTouchHandler partTouchHandler = [] (Part*, Part*, Vec3) {};
	PartReleaseHandler partReleaseHandler = [] (Part*, Part*) {};
	
	void* getPtr() const;
	void setPtr(void* ptr);
	void setCameraMoveCallback(CameraMoveHandler handler);
	void setWindowResizeCallback(WindowResizeHandler handler);
	void setPartRayIntersectCallback(PartRayIntersectHandler handler);
	void setPartDragCallback(PartDragHandler handler);
	void setPartClickCallback(PartClickHandler handler);
	void setPartTouchCallback(PartTouchHandler handler);
	void setPartReleaseCallback(PartReleaseHandler handler);
};
