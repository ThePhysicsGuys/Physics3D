#pragma once

#include "imgui/imgui.h"

namespace ImGui {
    bool InputDouble3(const char* label, double v[3], ImGuiInputTextFlags flags) {
        char format[16] = "%f";
        // TODO add format
        return InputScalarN(label, ImGuiDataType_Double, v, 3, NULL, NULL, format, flags);
    }
}