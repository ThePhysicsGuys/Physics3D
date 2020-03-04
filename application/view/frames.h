#pragma once

#include "../util/resource/resource.h"
#include "../graphics/gui/color.h"
#include "../engine/layer/layer.h"
#include "../graphics/texture.h"
#include "../graphics/font.h"
#include "../graphics/shader/shader.h"

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
	static void renderShaderStageInfo(Shader* shader, const ShaderStage& stage);
	static void renderTextureInfo(Graphics::Texture* texture);
	static void renderFontInfo(Graphics::Font* font);
	static void renderShaderInfo(Shader* shader);
	
	static void renderPropertiesFrame();
	static void renderDebugFrame();
	static void renderEnvironmentFrame();
	static void renderLayerFrame();
	static void renderResourceFrame();

public:
	static void render();

};

};