#pragma once

#include "../input/keyboard.h"
#include "../util/properties.h"

namespace P3D::Engine {

namespace KeyboardOptions {
	using Engine::Key;

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
		extern Key select;
		extern Key region;
	};

	namespace Part {
		extern Key remove;
		extern Key anchor;
		extern Key makeMainPart;
		extern Key makeMainPhysical;
	}

	namespace Application {
		extern Key close;
	}

	namespace World {
		extern Key valid;
	}

	void load(const Util::Properties& properties);
	void save(Util::Properties& properties);
}

};