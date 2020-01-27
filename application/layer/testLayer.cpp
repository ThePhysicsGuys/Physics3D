#include "core.h"

#include "testLayer.h"
#include "view/screen.h"

#include "../engine/ecs/entity.h"
#include "ecs/model.h"
#include "ecs/transform.h"

#include "../graphics/shader/shaderLexer.h"

namespace Application {

void TestLayer::onInit() {
	Entity* entity = new Entity();
	TransformComp* t = new TransformComp(GlobalCFrame());
	Model* m = new Model();

	entity->addComponent(t);
	entity->addComponent(m);
}

void TestLayer::onUpdate() {

}

void TestLayer::onEvent(Event& event) {

}

void TestLayer::onRender() {

}

void TestLayer::onClose() {

}

};