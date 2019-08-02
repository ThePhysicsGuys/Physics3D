#pragma once

class Screen;
class GravityWorld;

extern GravityWorld world;
extern Screen screen;

void init();

void setupPhysics();
void setupWorld();
void setupScreen();
void setupDebug();

void pause();
void unpause();
bool isPaused();
void togglePause();
void runTick();
void setSpeed(double newSpeed);
double getSpeed();

void stop(int returnCode);

void toggleFlying();

