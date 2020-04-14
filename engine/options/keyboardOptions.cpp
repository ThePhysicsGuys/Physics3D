#include "core.h"

#include "keyboardOptions.h"

namespace KeyboardOptions {

	namespace Move {
		Key forward = Keyboard::UNKNOWN;
		Key backward = Keyboard::UNKNOWN;
		Key right = Keyboard::UNKNOWN;
		Key left = Keyboard::UNKNOWN;
		Key jump = Keyboard::UNKNOWN;
		Key fly = Keyboard::UNKNOWN;
		Key ascend = Keyboard::UNKNOWN;
		Key descend = Keyboard::UNKNOWN;
	};

	namespace Rotate {
		Key up = Keyboard::UNKNOWN;
		Key down = Keyboard::UNKNOWN;
		Key left = Keyboard::UNKNOWN;
		Key right = Keyboard::UNKNOWN;
	};

	namespace Tick {
		Key run = Keyboard::UNKNOWN;
		Key pause = Keyboard::UNKNOWN;
		namespace Speed {
			Key up = Keyboard::UNKNOWN;
			Key down = Keyboard::UNKNOWN;
		};
	};

	namespace Debug {
		Key spheres = Keyboard::UNKNOWN;
		Key tree = Keyboard::UNKNOWN;
		Key pies = Keyboard::UNKNOWN;
		Key frame = Keyboard::UNKNOWN;
	};

	namespace Edit {
		Key translate = Keyboard::UNKNOWN;
		Key rotate = Keyboard::UNKNOWN;
		Key scale = Keyboard::UNKNOWN;
	};

	namespace Part {
		Key remove = Keyboard::UNKNOWN;
		Key anchor = Keyboard::UNKNOWN;
		Key makeMainPart = Keyboard::UNKNOWN;
		Key makeMainPhysical = Keyboard::UNKNOWN;
	}

	namespace Application {
		Key close = Keyboard::UNKNOWN;
	}

	namespace World {
		Key valid = Keyboard::UNKNOWN;
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

		// Application
		saveKey(properties, "application.close", Application::close);

		// World
		saveKey(properties, "world.valid", World::valid);
	}
}