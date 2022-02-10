#pragma once

#include "worlds.h"
#include "view/screen.h"

namespace P3D::Application {
void buildBenchmarkWorld(PlayerWorld& world);
void buildShowcaseWorld(Screen& screen, PlayerWorld& world);
void buildDebugWorld(Screen& screen, PlayerWorld& world);
void buildBallWorld(Screen& screen, PlayerWorld& world);
};
