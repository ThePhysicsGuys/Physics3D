#pragma once

class Event;
namespace Application {

class Screen;
class PlayerWorld;

extern PlayerWorld world;
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
void onEvent(::Event& event);

};