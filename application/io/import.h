#pragma once

#include "worlds.h"

struct ExtendedPart;
class Screen;

namespace WorldImport {
	void load(std::string name, World<ExtendedPart>& world, Screen& screen, bool binary = false);
};