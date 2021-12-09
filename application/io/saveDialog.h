#pragma once

namespace P3D::Application {

constexpr size_t MAX_PATH_LENGTH = 1024;
// returns true if user selected save
bool saveWorldDialog(char* worldFilePath);
// returns true if user selected open
bool openWorldDialog(char* worldFilePath);
};