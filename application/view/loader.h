#pragma once

#include <istream>
#include <string>

#include "indexedMesh.h"

Shape loadObj(std::string filename);
Shape loadObj(std::istream& filename);
void saveObj(std::string filename, Shape shape);