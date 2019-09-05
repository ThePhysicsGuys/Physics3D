#pragma once

#include "editTools.h"

class Screen;

namespace Picker {
	
	extern EditTools editTools;

	void init();
	void update(Screen& screen, Vec2 mousePosition);
	void render(Screen& screen);
	void close();

	void press(Screen& screen);
	void release(Screen& screen);
	void drag(Screen& screen);

	void moveGrabbedPhysicalTransversal(Screen& screen, double dz);
	void moveGrabbedPhysicalLateral(Screen& screen);
}