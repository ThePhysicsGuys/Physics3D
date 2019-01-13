#pragma once

void stop(int returnCode);
void pause();
void unpause();
bool isPaused();
void togglePause();
void setSpeed(double newSpeed);
double getSpeed();
void runTick();
