#pragma once

#include "editTools.h"

class Screen;

namespace Picker {
	
	extern EditTools editTools;

	void onInit();
	void onUpdate(Screen& screen, Vec2 mousePosition);
	void onRender(Screen& screen);
	void onClose();

	void press(Screen& screen);
	void release(Screen& screen);
	void drag(Screen& screen);

	void moveGrabbedPhysicalTransversal(Screen& screen, double dz);
	void moveGrabbedPhysicalLateral(Screen& screen);
}