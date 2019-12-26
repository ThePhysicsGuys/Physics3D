#include "core.h"
#include "frames.h"

#pragma region EnvironmentFrame

bool EnvironmentFrame::hdr = true;
float EnvironmentFrame::gamma = 1.0f;
float EnvironmentFrame::exposure = 1.0f;
Color3 EnvironmentFrame::sunColor = Color3(1);

#pragma endregion

#pragma region PropertiesFrame

float PropertiesFrame::position[3] = { 0, 0, 0 };

#pragma endregion