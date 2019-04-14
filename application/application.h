#pragma once

#include "../engine/math/vec3.h"
#include "../engine/math/mat3.h"
#include "../engine/part.h"
#include "gui/camera.h"

void stop(int returnCode);
void pause();
void unpause();
bool isPaused();
void togglePause();
void setSpeed(double newSpeed);
double getSpeed();
void runTick();
bool isPaused();
void createDominoAt(Vec3 pos, Mat3 rotation);
extern Part* player;
extern bool flying;
void toggleFlying();
Camera& getCamera();
