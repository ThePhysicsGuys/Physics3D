#pragma once

#include "../../resource/textureResource.h"
#include "../util/resource/resourceManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "../engine/tool/tool.h"

namespace ImGui {

	inline void DrawLineBetween(const ImRect& button1, const ImRect& button2, float yExtent = 0) {
		ImVec2 pos1 = ImVec2(
			button1.Max.x + (button2.Min.x - button1.Max.x) / 2,
			ImMin(button1.Min.y, button2.Min.y) - yExtent / 2
		);
		ImVec2 pos2 = ImVec2(
			pos1.x,
			ImMax(button1.Max.y, button2.Max.y) + yExtent / 2
		);

		GImGui->CurrentWindow->DrawList->AddLine(pos1, pos2, ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 0.3f)));
	}

	inline void DrawLine(const ImVec2& pos1, const ImVec2& pos2) {
		GImGui->CurrentWindow->DrawList->AddLine(pos1, pos2, ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 0.3f)));
	}

	// Draw buttons
	inline void DrawButton(const ImRect& button, bool even, bool selected, bool held, bool hovered, float rounding = 0.0f) {
		ImU32 color = even ? ImGui::GetColorU32(ImVec4(0, 0, 0, 0)) : ImGui::GetColorU32(ImVec4(0.4f, 0.4f, 0.4f, 0.1f));
		if (held || selected)
			color = ImGui::GetColorU32(ImGuiCol_ButtonActive);
		else if (hovered)
			color = ImGui::GetColorU32(ImGuiCol_ButtonHovered);

		GImGui->CurrentWindow->DrawList->AddRectFilled(ImVec2(button.Min.x, button.Min.y - GImGui->Style.ItemSpacing.y / 2), ImVec2(button.Max.x, button.Max.y + GImGui->Style.ItemSpacing.y / 2), color, rounding);
	}

	// Draw icons
	inline void DrawIcon(GLID icon, const ImVec2& min, const ImVec2& max) {
		if (icon == 0)
			return;

		GImGui->CurrentWindow->DrawList->AddImage(reinterpret_cast<ImTextureID>(icon), min, max);
	}

	inline ImU32 HexToImU32(const char* hex) {
		int i[4];
		sscanf(hex, "%02X%02X%02X%02X", (unsigned int*) &i[0], (unsigned int*) &i[1], (unsigned int*) &i[2], (unsigned int*) &i[3]);

		ImVec4 col = {
			i[0] / 255.0f,
			i[1] / 255.0f,
			i[2] / 255.0f,
			i[3] / 255.0f,
		};

		return ImGui::ColorConvertFloat4ToU32(col);
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

    inline bool DragVecN(const char* id, const char** labels, float* data, int components, float resetValue = 0.0f, float* speed = nullptr, bool resetAll = false, float** min = nullptr, float** max = nullptr, ImU32* colors = nullptr) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

		if (data == nullptr)
			return false;;

        bool result = false;
    	
        float size = GImGui->Style.ItemInnerSpacing.x * 2.0f;
        for (int i = 0; i < components; i++) {
            float label_size = CalcTextSize(labels[i], nullptr, true).x;
            float padding_size = GImGui->Style.FramePadding.x * 2.0f;
            float spacing_size = GImGui->Style.ItemInnerSpacing.x * 2.0f;
            size += label_size + padding_size + spacing_size;
        }

		if (resetAll) {
            float label_size = CalcTextSize("Reset", nullptr, true).x;
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

    		if (colors) {
				PushStyleColor(ImGuiCol_Button, colors[3 * i + 0]);
				PushStyleColor(ImGuiCol_ButtonHovered, colors[3 * i + 1]);
				PushStyleColor(ImGuiCol_ButtonActive, colors[3 * i + 2]);
			}

            if(Button(labels[i])) {
                *(data + i) = resetValue;
                result = true;
            }

			if (colors)
				PopStyleColor(3);

			SameLine(0, GImGui->Style.ItemInnerSpacing.x);

            PushID(i);
            result |= DragScalar("", ImGuiDataType_Float, data + i, speed == nullptr ? 0.1f : *(speed + i), min == nullptr ? nullptr : *(min + i), max == nullptr ? nullptr : *(max + i), "%.2f");
            PopID();

	
    		
            PopItemWidth();
    	}

		if (resetAll) {
            SameLine(0, GImGui->Style.ItemInnerSpacing.x);

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

    inline bool DragVec3(const char* id, float values[3], float resetValue = 0.0f, float* speed = nullptr, bool resetAll = false, float** min = nullptr, float** max = nullptr) {
        const char* labels[] = { "X", "Y", "Z" };
		ImU32 colors[] = {
			4280887473, // Red Idle
			4280097889, // Red Hover
			4280821893, // Red Active

			4281503782, // Green Idle
			4280574236, // Green Hover
			4281566504, // Green Active
			
			4289808168, // Blue Idle
			4284558364, // Blue Hover
			4286922280, // Blue Active
		};

        return DragVecN(id, labels, values, 3, resetValue, speed, resetAll, min, max, colors);
    }

    inline void HelpMarker(const char* description) {
        TextDisabled("(?)");
        if (IsItemHovered()) {
            BeginTooltip();
            PushTextWrapPos(GetFontSize() * 35.0f);
            TextUnformatted(description);
            PopTextWrapPos();
            EndTooltip();
        }
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
        ImGui::PushID(name);
        bool result = ImageButton((ImTextureID) image->getID(), ImVec2(25, 25), ImVec2(0, 1), ImVec2(1, 0), 2, bg);
        ImGui::PopID();
		
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
        SameLine(0, 30);
        Spacing();
	}
	
    inline void EndToolBar() {
        End();
	}
	
}

#define PROPERTY_FRAME_START(label) \
	if (ImGui::CollapsingHeader((label), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap)) { \
		ImGui::Columns(2)

#define PROPERTY_FRAME_END\
		ImGui::Columns(1); \
	}

#define PROPERTY_DESC_IF(text, desc, widget, code) \
	{ \
		ImGui::TextUnformatted(text); \
		ImGui::SameLine(); \
		ImGui::HelpMarker(desc); \
		ImGui::NextColumn(); \
		ImGui::SetNextItemWidth(-1); \
		if (widget) { \
			code \
		}; \
		ImGui::NextColumn(); \
	}

#define PROPERTY_IF(text, widget, code) \
	{ \
		ImGui::TextUnformatted(text); \
		ImGui::NextColumn(); \
		ImGui::SetNextItemWidth(-1); \
		if (widget) { \
			code \
		}; \
		ImGui::NextColumn(); \
	}

#define PROPERTY_DESC_IF_LOCK(text, desc, widget, code) \
	{ \
		ImGui::TextUnformatted(text); \
		ImGui::SameLine(); \
		ImGui::HelpMarker(desc); \
		ImGui::NextColumn(); \
		ImGui::SetNextItemWidth(-1); \
		if (widget) { \
			screen.worldMutex->lock() \
			code \
			screen.worldMutex->unlock() \
		}; \
		ImGui::NextColumn(); \
	}

#define PROPERTY_IF_LOCK(text, widget, code) \
	{ \
		ImGui::TextUnformatted(text); \
		ImGui::NextColumn(); \
		ImGui::SetNextItemWidth(-1); \
		if (widget) { \
			screen.worldMutex->lock(); \
			code \
			screen.worldMutex->unlock(); \
		}; \
		ImGui::NextColumn(); \
	}

#define PROPERTY_DESC(text, desc, widget) \
	{ \
		ImGui::TextUnformatted(text); \
		ImGui::SameLine(); \
		ImGui::HelpMarker(desc); \
		ImGui::NextColumn(); \
		ImGui::SetNextItemWidth(-1); \
		widget; \
		ImGui::NextColumn(); \
	} 

#define PROPERTY(text, widget) \
	{ \
		ImGui::TextUnformatted(text); \
		ImGui::NextColumn(); \
		ImGui::SetNextItemWidth(-1); \
		widget; \
		ImGui::NextColumn(); \
	} 

#define TITLE_DESC(text, desc, newline) \
	{ \
		if (newline) { \
			ECS_PROPERTY("", ); \
		} \
		ImGui::TextColored(GImGui->Style.Colors[ImGuiCol_ButtonActive], text); \
		ImGui::SameLine(); \
		ImGui::HelpMarker(desc); \
		ImGui::NextColumn(); \
		ImGui::SetNextItemWidth(-1); \
		ImGui::NextColumn(); \
	}

#define TITLE(text, newline) \
	{ \
		if (newline) { \
			PROPERTY("", ); \
		} \
		ImGui::TextColored(GImGui->Style.Colors[ImGuiCol_ButtonActive], text); \
		ImGui::NextColumn(); \
		ImGui::SetNextItemWidth(-1); \
		ImGui::NextColumn(); \
	}