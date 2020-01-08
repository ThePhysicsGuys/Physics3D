#pragma once

#include "../util/resource/resource.h"
#include "../graphics/gui/color.h"
#include "../engine/layer/layer.h"
#include "../graphics/texture.h"
#include "../graphics/font.h"

#include "imgui/imgui.h"

namespace Application {

struct BigFrame {
public:
	// PropertiesFrame
	static float position[3];

	// DebugFrame
	static bool renderSpheres;

	// EnvironmentFrame
	static bool hdr;
	static float gamma;
	static float exposure;
	static Color3 sunColor;

	// LayerFrame
	static bool doEvents;
	static bool noRender;
	static bool doUpdate;
	static bool isDisabled;
	static Layer* selectedLayer;

	// ResourceFrame
	static Resource* selectedResource;

private:
	static void renderTextureInfo(Texture* texture);
	static void renderFontInfo(Font* font);
	
	static void renderPropertiesFrame();
	static void renderDebugFrame();
	static void renderEnvironmentFrame();
	static void renderLayerFrame();
	static void renderResourceFrame();

public:
	static void render();

};

};