#pragma once

#include "worlds.h"

namespace Application {

struct Camera;
class ExtendedPart;

namespace WorldExport {
void save(const std::string& name, World<ExtendedPart>& world, Camera& camera, bool binary = false);
};

};