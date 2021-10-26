#pragma once

#include <string>
#include "extendedPart.h"

#include "../worlds.h"
#include "../extendedPart.h"

namespace P3D::Application {

namespace WorldImportExport {

void registerTexture(Graphics::Texture* texture);

void saveWorld(const char* fileName, const PlayerWorld& world);
void saveLooseParts(const char* fileName, size_t numberOfParts, const ExtendedPart* const parts[]);


void loadWorld(const char* fileName, PlayerWorld& world);
void loadLoosePartsIntoWorld(const char* fileName, PlayerWorld& world);
void loadNativePartsIntoWorld(const char* fileName, PlayerWorld& world);

};

};