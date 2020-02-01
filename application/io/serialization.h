#pragma once

#include <string>
#include "extendedPart.h"

#include "../worlds.h"
#include "../extendedPart.h"

namespace Application {
namespace WorldImportExport {
void saveWorld(const char* fileName, const World<ExtendedPart>& world);
void saveLooseParts(const char* fileName, size_t numberOfParts, const ExtendedPart* const parts[]);


void loadWorld(const char* fileName, World<ExtendedPart>& world);
void loadLoosePartsIntoWorld(const char* fileName, World<ExtendedPart>& world);
void loadNativePartsIntoWorld(const char* fileName, World<ExtendedPart>& world);
};
};