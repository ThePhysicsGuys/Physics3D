#pragma once

#include "../graphics/gui/color.h"
#include "../engine/layer/layer.h"
#include "../engine/layer/layerStack.h"

namespace Graphics {
class Shader;
struct ShaderStage;
class Texture;
class Font;
}

namespace Engine {
class Node;
}

class Resource;

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
	static void renderECSNode(Engine::Node* node);
	static void renderShaderStageInfo(Graphics::Shader* shader, const Graphics::ShaderStage& stage);
	static void renderTextureInfo(Graphics::Texture* texture);
	static void renderFontInfo(Graphics::Font* font);
	static void renderShaderInfo(Graphics::Shader* shader);
	
	static void renderECSTree();
	static void renderPropertiesFrame();
	static void renderDebugFrame();
	static void renderEnvironmentFrame();
	static void renderLayerFrame();
	static void renderResourceFrame();

public:
	static void onInit();
	static void render();

};

};