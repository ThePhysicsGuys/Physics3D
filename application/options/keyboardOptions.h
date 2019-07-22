#pragma once

#include "../input/keyboard.h"
#include "../util/properties.h"

namespace KeyboardOptions {

	namespace Move {
		extern Key forward;
		extern Key backward;
		extern Key right;
		extern Key left;

		extern Key jump;
		extern Key fly;
		extern Key ascend;
		extern Key descend;
	};

	namespace Rotate {
		extern Key up;
		extern Key down;
		extern Key left;
		extern Key right;
	}

	namespace Tick {
		extern Key pause;
		extern Key run;
		namespace Speed {
			extern Key up;
			extern Key down;
		};
	};

	namespace Debug {
		extern Key spheres;
		extern Key tree;
		extern Key pies;
		extern Key frame;
	};

	namespace Edit {
		extern Key translate;
		extern Key rotate;
		extern Key scale;
	};

	namespace Part {
		extern Key remove;
		extern Key anchor;
	}

	namespace Application {
		extern Key close;
	}

	namespace World {
		extern Key valid;
	}

	void load(const Properties& properties);
	void save(Properties properties);
}