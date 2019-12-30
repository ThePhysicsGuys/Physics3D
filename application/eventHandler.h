#pragma once

namespace Application {

class Screen;
struct Camera;
struct ExtendedPart;

typedef void (*CameraMoveHandler) (Screen&, Camera*, Vec3);
typedef void (*WindowResizeHandler) (Screen&, Vec2i);
typedef void (*PartRayIntersectHandler) (Screen&, ExtendedPart*, Position);
typedef void (*PartDragHandler) (Screen&, ExtendedPart*, Vec3);
typedef void (*PartClickHandler) (Screen&, ExtendedPart*, Vec3);
typedef void (*PartTouchHandler) (ExtendedPart*, ExtendedPart*, Vec3);
typedef void (*PartReleaseHandler) (ExtendedPart*, ExtendedPart*);

class EventHandler {
private:
	void* pointer = nullptr;

public:
	CameraMoveHandler cameraMoveHandler = [] (Screen&, Camera*, Vec3) {};
	WindowResizeHandler windowResizeHandler = [] (Screen&, Vec2i) {};
	PartRayIntersectHandler partRayIntersectHandler = [] (Screen&, ExtendedPart*, Position) {};
	PartDragHandler partDragHandler = [] (Screen&, ExtendedPart*, Vec3) {};
	PartClickHandler partClickHandler = [] (Screen&, ExtendedPart*, Vec3) {};
	PartTouchHandler partTouchHandler = [] (ExtendedPart*, ExtendedPart*, Vec3) {};
	PartReleaseHandler partReleaseHandler = [] (ExtendedPart*, ExtendedPart*) {};

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

}