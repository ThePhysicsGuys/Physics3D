#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#define IMGUI_DEFINE_MATH_OPERATORS

namespace ImGui {

	ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) {
        return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}
	
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

    bool DragVecN(const char* id, const char** labels, float* data, int components, float resetValue = 0.0f, float speed = 0.1f, bool resetAll = false) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        bool result = false;
    	
        float size = GImGui->Style.ItemInnerSpacing.x * 2.0f;
        for (int i = 0; i < components; i++) {
            float label_size = CalcTextSize(labels[i], NULL, true).x;
            float padding_size = GImGui->Style.FramePadding.x * 2.0f;
            float spacing_size = GImGui->Style.ItemInnerSpacing.x * 2.0f;
            size += label_size + padding_size + spacing_size;
        }

		if (resetAll) {
            float label_size = CalcTextSize("Reset", NULL, true).x;
            float padding_size = GImGui->Style.FramePadding.x * 2.0f;
            float spacing_size = GImGui->Style.ItemInnerSpacing.x * 2.0f;
            size += label_size + padding_size + spacing_size;
		}
    	
        BeginGroup();
    	
        PushMultiItemsWidths(components, GetColumnWidth() - size);

        PushID(id);

    	for (int i = 0; i < components; i++) {

            if (i > 0)
                SameLine(0, GImGui->Style.ItemInnerSpacing.x);

            if(Button(labels[i])) {
                *(data + i) = resetValue;
                result = true;
            }    		
    		
			SameLine(0, GImGui->Style.ItemInnerSpacing.x);
    		
            PushID(i);
            result |= DragScalar("", ImGuiDataType_Float, data + i, speed, 0, 0, "%.2f");
            PopID();
    		
            PopItemWidth();
    	}

		if (resetAll) {
            SameLine(0, GImGui->Style.ItemInnerSpacing.x);
			if (Button("Reset")) {
				for (int i = 0; i < components; i++)
                    *(data + i) = resetValue;
                result = true;
			}
		}

        PopID();

        EndGroup();
    	
        return result;
    }

    bool DragVec3(const char* id, float values[3], float resetValue = 0.0f, float speed = 0.1f, bool resetAll = false) {
        const char* labels[] = { "X", "Y", "Z" };
        return DragVecN(id, labels, values, 3, resetValue, speed, resetAll);
    }
	
}
