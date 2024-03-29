#pragma once

#include <Physics3D/threading/upgradeableMutex.h>

namespace P3D::Engine {
struct Event;
};

namespace P3D::Application {

class Screen;
class PlayerWorld;

extern PlayerWorld world;
extern UpgradeableMutex worldMutex;
extern Screen screen;

void pause();
void unpause();
bool isPaused();
void togglePause();
void runTick();
void setSpeed(double newSpeed);
double getSpeed();
void stop(int returnCode);
void toggleFlying();
void onEvent(Engine::Event& event);

};