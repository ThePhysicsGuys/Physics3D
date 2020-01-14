#include "core.h"

#include "testLayer.h"
#include "view/screen.h"

#include "ecs/entity.h";
#include "ecs/components/model.h"
#include "ecs/components/transform.h"

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