#pragma once

#include "editTools.h"

namespace P3D::Engine {
class Event;
class MousePressEvent;
class MouseReleaseEvent;
class MouseDragEvent;
};

namespace P3D::Application {

class Screen;

namespace Picker {

extern EditTools editTools;

void onInit();
void onEvent(Engine::Event& event);
void onUpdate(Screen& screen, Vec2 mousePosition);
void onRender(Screen& screen);
void onClose();

bool onMousePress(Engine::MousePressEvent& event);
bool onMouseRelease(Engine::MouseReleaseEvent& event);
bool onMouseDrag(Engine::MouseDragEvent& event);

void moveGrabbedEntityTransversal(Screen& screen, double dz);
void moveGrabbedEntityLateral(Screen& screen);
}

};