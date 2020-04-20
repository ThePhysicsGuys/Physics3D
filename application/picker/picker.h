#pragma once

#include "editTools.h"

namespace Engine {
class Event;
class MousePressEvent;
class MouseReleaseEvent;
class MouseDragEvent;
};

namespace Application {

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

void moveGrabbedPhysicalTransversal(Screen& screen, double dz);
void moveGrabbedPhysicalLateral(Screen& screen);
}

};