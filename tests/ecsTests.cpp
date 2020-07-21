#include "testsMain.h"

#include "compare.h"
#include "../engine/ecs/registry.h"

TEST_CASE(idGeneration) {
	using namespace Engine;
	Registry8 registry;
	auto id1 = registry.create();
	auto id2 = registry.create();
	registry.destroy(id1);
	auto id3 = registry.create();

	struct A {};
	struct B {};

	registry.add<A>(id2);
	registry.add<B>(id2);
	registry.remove<A>(id2);

}