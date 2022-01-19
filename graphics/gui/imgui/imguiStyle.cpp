#include "core.h"
#include "imguiStyle.h"

namespace P3D::Graphics {

ImGuiColors theme = Dark;
ImVec4 text = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
ImVec4 idle = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
ImVec4 hover = ImVec4(0.10f, 0.40f, 0.75f, 0.75f);
ImVec4 active = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
ImVec4 dark = ImVec4(0.176f, 0.228f, 0.435f, 1.00f);

void renderImGuiStyleEditor() {
    ImGui::Begin("Style");
    ImGuiStyle* style = &ImGui::GetStyle();

    const char* themes[] = { "Classic", "Classic Light", "Classic Dark", "Light", "Gray", "Dark" };
    const char* current = themes[theme];
    ImGui::Text("Theme");
    if (ImGui::BeginCombo("Theme", current)) {
        for (int i = 0; i < IM_ARRAYSIZE(themes); i++) {
            bool is_selected = i == theme;
            if (ImGui::Selectable(themes[i], is_selected)) {
                theme = static_cast<ImGuiColors>(i);
                setupImGuiColors(style, theme);

            	if (i >= 3) 
                    setupImGuiAccent(style);
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    bool edited = false;
    if (theme >= 3) {
        ImGui::Text("Accent");
        edited |= ImGui::ColorEdit4("Text", (float*) &text, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaBar);
        edited |= ImGui::ColorEdit4("Idle", (float*) &idle, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaBar);
        edited |= ImGui::ColorEdit4("Hover", (float*) &hover, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaBar);
        edited |= ImGui::ColorEdit4("Active", (float*) &active, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaBar);
        edited |= ImGui::ColorEdit4("Dark", (float*) &dark, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaBar);

        if (edited)
            setupImGuiAccent(style);
    }
	
    ImGui::End();
}
	
void setupImGuiLayoutStyle(ImGuiStyle* style) {
    ImGuiIO& io = ImGui::GetIO();

    io.Fonts->AddFontFromFileTTF("../res/fonts/droid.ttf", 20);

    style->WindowPadding = ImVec2(8, 10);
    style->FramePadding = ImVec2(10, 5);
    style->ItemSpacing = ImVec2(15, 8);
    style->ItemInnerSpacing = ImVec2(6, 6);
    style->IndentSpacing = 20.0f;
    style->ScrollbarSize = 20.0f;
    style->GrabMinSize = 20.0f;

    style->WindowBorderSize = 0.0f;
    style->ChildBorderSize = 1.0f;
    style->FrameBorderSize = 0.0f;
    style->PopupBorderSize = 0.0f;
    style->TabBorderSize = 0.0f;

    style->WindowRounding = 6.0f;
    style->ChildRounding = 10.0f;
    style->FrameRounding = 6.0f;
    style->PopupRounding = 6.0f;
    style->ScrollbarRounding = 3.0f;
    style->GrabRounding = 5.0f;

    style->WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style->WindowMenuButtonPosition = ImGuiDir_Right;
}

void setupImGuiAccent(ImGuiStyle* style) {
	style->Colors[ImGuiCol_Text] = text;
	style->Colors[ImGuiCol_FrameBgHovered] = idle;
    style->Colors[ImGuiCol_CheckMark] = active;
    style->Colors[ImGuiCol_SliderGrab] = active;
    style->Colors[ImGuiCol_SliderGrabActive] = active;
    style->Colors[ImGuiCol_Button] = idle;
    style->Colors[ImGuiCol_ButtonHovered] = hover;
    style->Colors[ImGuiCol_ButtonActive] = active;
    style->Colors[ImGuiCol_HeaderHovered] = hover;
    style->Colors[ImGuiCol_HeaderActive] = active;
    style->Colors[ImGuiCol_SeparatorHovered] = hover;
    style->Colors[ImGuiCol_SeparatorActive] = active;
    style->Colors[ImGuiCol_ResizeGrip] = idle;
    style->Colors[ImGuiCol_ResizeGripHovered] = hover;
    style->Colors[ImGuiCol_ResizeGripActive] = active;
    style->Colors[ImGuiCol_TabHovered] = hover;
    style->Colors[ImGuiCol_TabActive] = active;
    style->Colors[ImGuiCol_DockingPreview] = hover;
    style->Colors[ImGuiCol_TextSelectedBg] = hover;
    style->Colors[ImGuiCol_NavHighlight] = active;
    style->Colors[ImGuiCol_Tab] = idle;
    style->Colors[ImGuiCol_TabUnfocusedActive] = dark;
    style->Colors[ImGuiCol_TabUnfocused] = active;
}

void setupImGuiColors(ImGuiStyle* style, ImGuiColors colors) {
    switch (colors) {
        case Light:

            break;
        case Gray:
            style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
            style->Colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
            style->Colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
            style->Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
            style->Colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
            style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            style->Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
            style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
            style->Colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
            style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
            style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
            style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
            style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
            style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
            style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
            style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
            style->Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
            style->Colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
            style->Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
            style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
            style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
            style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
            style->Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
            style->Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
            style->Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
            style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
            break;
        case Dark:
            style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
            style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
            style->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
            style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
            style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
            style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
            style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
            style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
            style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
            style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
            style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
            style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
            style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
            style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
            style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
            style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
            style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
            style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
            style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
            style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
            style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
            style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.75f);
            break;
        case ClassicLight:
            ImGui::StyleColorsLight(style);
            break;
        case ClassicDark:
            ImGui::StyleColorsDark(style);
            break;
        case Classic:
            ImGui::StyleColorsClassic(style);
    }
}

void setupImGuiStyle() {
    ImGuiStyle* style = &ImGui::GetStyle();
    setupImGuiLayoutStyle(style);
    setupImGuiAccent(style);
    setupImGuiColors(style, ImGuiColors::Dark);
}

}
