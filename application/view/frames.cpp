#include "core.h"
#include "frames.h"

namespace Application {

bool BigFrame::hdr = true;
float BigFrame::gamma = 1.0f;
float BigFrame::exposure = 1.0f;
Color3 BigFrame::sunColor = Color3(1);
float BigFrame::position[3] = { 0, 0, 0 };
bool BigFrame::doEvents;
bool BigFrame::noRender;
bool BigFrame::doUpdate;
bool BigFrame::isDisabled;
Layer* BigFrame::selectedLayer = nullptr;

}
