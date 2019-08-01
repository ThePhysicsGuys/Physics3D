#pragma once

#include "../engine/math/vec3.h"
#include "../engine/math/mat3.h"
#include "../engine/part.h"
#include "view/camera.h"
#include "extendedPart.h"

class Screen;
class GravityWorld;

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
extern ExtendedPart* player;
extern Screen screen;
extern GravityWorld world;
extern bool flying;
void toggleFlying();
Camera& getCamera();
