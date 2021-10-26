#pragma once

#include "../engine/ecs/registry.h"

namespace P3D::Graphics {
	class Texture;
	class Font;
	struct ShaderStage;
	class ShaderResource;
}

class Resource;

namespace P3D::Application {
	
struct ResourceFrame {
private:
	static void renderTextureInfo(Graphics::Texture* texture);
	static void renderFontInfo(Graphics::Font* font);
	static void renderShaderInfo(Graphics::ShaderResource* shader);
	
public:
	static Resource* selectedResource;

	static void onInit(Engine::Registry64& registry);
	static void onRender(Engine::Registry64& registry);

};
	
}
