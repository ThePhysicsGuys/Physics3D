#pragma once

#include "../../resource/textureResource.h"
#include "../util/resource/resourceManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "../engine/tool/tool.h"

#define IMGUI_DEFINE_MATH_OPERATORS

namespace ImGui {

	inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) {
        return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}
	
    inline bool InputDouble3(const char* label, double v[3], ImGuiInputTextFlags flags) {
        char format[16] = "%f";
        return InputScalarN(label, ImGuiDataType_Double, v, 3, NULL, NULL, format, flags);
    }

    inline bool InputFloat9(const char* label, float v[9], ImGuiInputTextFlags flags) {
        char format[16] = "%f";
        bool i1 = InputFloat3("", v);
        bool i2 = InputFloat3("", v + 3);
        bool i3 = InputFloat3("", v + 6);

        return i1 | i2 | i3;
    }

    inline bool DragVecN(const char* id, const char** labels, float* data, int components, float resetValue = 0.0f, float speed = 0.1f, bool resetAll = false) {
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
			//if (Button("Reset")) {
			
            //PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
            PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, -1));
			if (ImageButton((ImTextureID) ResourceManager::get<P3D::Graphics::TextureResource>("reset")->getID(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0))) {
				for (int i = 0; i < components; i++)
                    *(data + i) = resetValue;
                result = true;
			}
			
            PopStyleVar(1);
		}

        PopID();

        EndGroup();
    	
        return result;
    }

    inline bool DragVec3(const char* id, float values[3], float resetValue = 0.0f, float speed = 0.1f, bool resetAll = false) {
        const char* labels[] = { "X", "Y", "Z" };
        return DragVecN(id, labels, values, 3, resetValue, speed, resetAll);
    }

    inline void BeginToolbar(const char* name) {
        Begin(name, 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
	}

    inline bool ToolBarButton(const char* name, const char* description, const char* resource, bool selected = false) {
        P3D::Graphics::TextureResource* image = ResourceManager::get<P3D::Graphics::TextureResource>(resource);
        if (image == nullptr)
            return false;

        PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
        PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, -1));

		SameLine();
        ImVec4 bg = selected ? ImVec4(0.2, 0.2, 0.2, 0.5) : ImVec4(0, 0, 0, 0);
        bool result = ImageButton((ImTextureID) image->getID(), ImVec2(25, 25), ImVec2(0, 1), ImVec2(1, 0), 2, bg);

		PopStyleVar(2);
		
        if (IsItemHovered()) {
            BeginTooltip();
            Text(name);
            TextDisabled(description);
            EndTooltip();
        }

        return result;
    }

    inline bool ToolBarButton(P3D::Engine::Tool* tool, bool selected = false) {
        return ToolBarButton(tool->getName().c_str(), tool->getDescription().c_str(), tool->getName().c_str(), selected);
    }

    inline void ToolBarSpacing() {
        SameLine();
        Spacing();
	}
	
    inline void EndToolBar() {
        End();
	}
}
