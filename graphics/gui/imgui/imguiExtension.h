#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#define IMGUI_DEFINE_MATH_OPERATORS

namespace ImGui {
    bool InputDouble3(const char* label, double v[3], ImGuiInputTextFlags flags) {
        char format[16] = "%f";
        // TODO add format
        return InputScalarN(label, ImGuiDataType_Double, v, 3, NULL, NULL, format, flags);
    }

	bool InputFloat9(const char* label, float v[9], ImGuiInputTextFlags flags) {
        char format[16] = "%f";
        bool i1 = InputFloat3("", v);
        bool i2 = InputFloat3("", v + 3);
        bool i3 = InputFloat3("", v + 6);

        return i1 | i2 | i3;
    }

    bool InputVec3(const char* id, float value[3], float resetValue = 0.0f, float speed = 0.1f) {
        PushID(id);
    	
        PushMultiItemsWidths(4, CalcItemWidth());
        PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, GImGui->Style.ItemSpacing.y));
        PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

        float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = ImVec2(lineHeight + 3.0f, lineHeight);

        bool result = false;
    	
    	// X
        if (Button("X", buttonSize)) {
            result = true;
            value[0] = resetValue;
        }

        SameLine();
        result |= DragFloat("##X", value, speed, 0, 0, "%.2f");
        PopItemWidth();
        SameLine();

    	// Y
        if (Button("Y", buttonSize)) {
            result = true;
            value[1] = resetValue;
        }

        SameLine();
        result |= DragFloat("##Y", value + 1, speed, 0, 0, "%.2f");
        PopItemWidth();
        SameLine();

    	// Z
        if (Button("Z", buttonSize)) {
            result = true;
            value[2] = resetValue;
        }

        SameLine();
        result |= DragFloat("##Z", value + 2, speed, 0, 0, "%.2f");
        PopItemWidth();

        PopStyleVar(2);
        PopID();
    	
        return result;
    }
	
}
