#pragma once

#include "editTools.h"

class Screen;
class Event;

class MousePressEvent;
class MouseReleaseEvent;
class MouseDragEvent;

namespace Picker {
	
	extern EditTools editTools;

	void onInit();
	void onEvent(Event& event);
	void onUpdate(Screen& screen, Vec2 mousePosition);
	void onRender(Screen& screen);
	void onClose();

	bool onMousePress(MousePressEvent& event);
	bool onMouseRelease(MouseReleaseEvent& event);
	bool onMouseDrag(MouseDragEvent& event);

	void moveGrabbedPhysicalTransversal(Screen& screen, double dz);
	void moveGrabbedPhysicalLateral(Screen& screen);
}