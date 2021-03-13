#include "core.h"

#include "keyboardOptions.h"

namespace P3D::Engine {

namespace KeyboardOptions {

	namespace Move {
		Key forward  = Keyboard::KEY_UNKNOWN;
		Key backward = Keyboard::KEY_UNKNOWN;
		Key right    = Keyboard::KEY_UNKNOWN;
		Key left     = Keyboard::KEY_UNKNOWN;
		Key jump     = Keyboard::KEY_UNKNOWN;
		Key fly      = Keyboard::KEY_UNKNOWN;
		Key ascend   = Keyboard::KEY_UNKNOWN;
		Key descend  = Keyboard::KEY_UNKNOWN;
	};

	namespace Rotate {
		Key up    = Keyboard::KEY_UNKNOWN;
		Key down  = Keyboard::KEY_UNKNOWN;
		Key left  = Keyboard::KEY_UNKNOWN;
		Key right = Keyboard::KEY_UNKNOWN;
	};

	namespace Tick {
		Key run   = Keyboard::KEY_UNKNOWN;
		Key pause = Keyboard::KEY_UNKNOWN;
		namespace Speed {
			Key up   = Keyboard::KEY_UNKNOWN;
			Key down = Keyboard::KEY_UNKNOWN;
		};
	};

	namespace Debug {
		Key spheres = Keyboard::KEY_UNKNOWN;
		Key tree    = Keyboard::KEY_UNKNOWN;
		Key pies    = Keyboard::KEY_UNKNOWN;
		Key frame   = Keyboard::KEY_UNKNOWN;
	};

	namespace Edit {
		Key translate = Keyboard::KEY_UNKNOWN;
		Key rotate    = Keyboard::KEY_UNKNOWN;
		Key scale     = Keyboard::KEY_UNKNOWN;
		Key select    = Keyboard::KEY_UNKNOWN;
		Key region    = Keyboard::KEY_UNKNOWN;
	};

	namespace Part {
		Key remove           = Keyboard::KEY_UNKNOWN;
		Key anchor           = Keyboard::KEY_UNKNOWN;
		Key makeMainPart     = Keyboard::KEY_UNKNOWN;
		Key makeMainPhysical = Keyboard::KEY_UNKNOWN;
	}

	namespace Application {
		Key close = Keyboard::KEY_UNKNOWN;
	}

	namespace World {
		Key valid = Keyboard::KEY_UNKNOWN;
	}

	Key loadKey(const Util::Properties& properties, std::string key) {
		return Keyboard::getKey(properties.get(key));
	}

	void saveKey(Util::Properties& properties, std::string property, const Key& key) {
		properties.set(property, key.getName());
	}

	void load(const Util::Properties& properties) {
		// Move
		Move::forward = loadKey(properties, "move.forward");
		Move::backward = loadKey(properties, "move.backward");
		Move::right = loadKey(properties, "move.right");
		Move::left = loadKey(properties, "move.left");
		Move::jump = loadKey(properties, "move.jump");
		Move::fly = loadKey(properties, "move.fly");
		Move::ascend = loadKey(properties, "move.ascend");
		Move::descend = loadKey(properties, "move.descend");

		// Rotate
		Rotate::up = loadKey(properties, "rotate.up");
		Rotate::down = loadKey(properties, "rotate.down");
		Rotate::left = loadKey(properties, "rotate.left");
		Rotate::right = loadKey(properties, "rotate.right");

		// Tick
		Tick::run = loadKey(properties, "tick.run");
		Tick::pause = loadKey(properties, "tick.pause");
		Tick::Speed::up = loadKey(properties, "tick.speed.up");
		Tick::Speed::down = loadKey(properties, "tick.speed.down");
	
		// Debug
		Debug::spheres = loadKey(properties, "debug.spheres");
		Debug::tree = loadKey(properties, "debug.tree");
		Debug::pies = loadKey(properties, "debug.pies");
		Debug::frame = loadKey(properties, "debug.frame");

		// Part
		Part::anchor = loadKey(properties, "part.anchor");
		Part::remove = loadKey(properties, "part.delete");
		Part::makeMainPart = loadKey(properties, "part.makeMainPart");
		Part::makeMainPhysical = loadKey(properties, "part.makeMainPhysical");

		// Edit
		Edit::translate = loadKey(properties, "edit.translate");
		Edit::rotate = loadKey(properties, "edit.rotate");
		Edit::scale = loadKey(properties, "edit.scale");
		Edit::select = loadKey(properties, "edit.select");
		Edit::region = loadKey(properties, "edit.region");

		// Application
		Application::close = loadKey(properties, "application.close");

		// World
		World::valid = loadKey(properties, "world.valid");
	}

	void save(Util::Properties& properties) {
		// Move
		saveKey(properties, "move.forward", Move::forward);
		saveKey(properties, "move.backward", Move::backward);
		saveKey(properties, "move.right", Move::right);
		saveKey(properties, "move.left", Move::left);
		saveKey(properties, "move.jump", Move::jump);
		saveKey(properties, "move.fly", Move::fly);
		saveKey(properties, "move.ascend", Move::ascend);
		saveKey(properties, "move.descend", Move::descend);

		// Rotate
		saveKey(properties, "rotate.up", Rotate::up);
		saveKey(properties, "rotate.down", Rotate::down);
		saveKey(properties, "rotate.left", Rotate::left);
		saveKey(properties, "rotate.right", Rotate::right);

		// Tick
		saveKey(properties, "tick.run", Tick::run);
		saveKey(properties, "tick.pause", Tick::pause);
		saveKey(properties, "tick.speed.up", Tick::Speed::up);
		saveKey(properties, "tick.speed.down", Tick::Speed::down);

		// Debug
		saveKey(properties, "debug.spheres", Debug::spheres);
		saveKey(properties, "debug.tree", Debug::tree);
		saveKey(properties, "debug.pies", Debug::pies);
		saveKey(properties, "debug.frame", Debug::frame);

		// Part
		saveKey(properties, "part.anchor", Part::anchor);
		saveKey(properties, "part.delete", Part::remove);

		// Edit
		saveKey(properties, "edit.translate", Edit::translate);
		saveKey(properties, "edit.rotate", Edit::rotate);
		saveKey(properties, "edit.scale", Edit::scale);
		saveKey(properties, "edit.select", Edit::select);
		saveKey(properties, "edit.region", Edit::region);

		// Application
		saveKey(properties, "application.close", Application::close);

		// World
		saveKey(properties, "world.valid", World::valid);
	}
}

};