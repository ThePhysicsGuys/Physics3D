#include "core.h"

#include "resourceFrame.h"

#include "imgui/imgui.h"
#include "shader/shaders.h"
#include "../util/resource/resourceManager.h"
#include "../graphics/gui/imgui/imguiExtension.h"
#include "../graphics/resource/fontResource.h"
#include "../graphics/resource/textureResource.h"
#include "../graphics/texture.h"
#include "../graphics/font.h"

namespace P3D::Application {

Resource* ResourceFrame::selectedResource = nullptr;
	
void ResourceFrame::renderTextureInfo(Texture* texture) {
	ImGui::Text("ID: %d", texture->getID());
	ImGui::Text("Width: %d", texture->getWidth());
	ImGui::Text("Height: %d", texture->getHeight());
	ImGui::Text("Channels: %d", texture->getChannels());
	ImGui::Text("Unit: %d", texture->getUnit());

	if (ImGui::TreeNodeEx("Preview", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen)) {
		float cw = ImGui::GetColumnWidth();
		float ch = ImGui::GetWindowHeight();
		float tw = ch * texture->getAspect();
		float th = cw / texture->getAspect();

		ImVec2 size;
		if (th > ch)
			size = ImVec2(tw, ch);
		else
			size = ImVec2(cw, th);

		Vec4f c = Colors::ACCENT;
		ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
		ImGui::Image((void*) (intptr_t) texture->getID(), size, ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(c.x, c.y, c.z, c.w));

		ImGui::TreePop();
	}
}

void ResourceFrame::renderFontInfo(Font* font) {
	if (ImGui::TreeNode("Atlas")) {
		renderTextureInfo(font->getAtlas());
		ImGui::TreePop();
	}

	char selectedCharacter = 0;
	if (ImGui::TreeNode("Characters")) {
		for (int i = 0; i < CHARACTER_COUNT; i++) {
			Character& character = font->getCharacter(i);

			float s = float(character.x) / font->getAtlasWidth();
			float t = float(character.y) / font->getAtlasHeight();
			float ds = float(character.width) / font->getAtlasWidth();
			float dt = float(character.height) / font->getAtlasHeight();

			ImGui::ImageButton((void*) (intptr_t) font->getAtlas()->getID(), ImVec2(character.width / 2.0f, character.height / 2.0f), ImVec2(s, t), ImVec2(s + ds, t + dt));
			float last_button_x2 = ImGui::GetItemRectMax().x;
			float next_button_x2 = last_button_x2 + ImGui::GetStyle().ItemSpacing.x + character.width / 2.0f; // Expected position if next button was on same line
			if (i + 1 < CHARACTER_COUNT && next_button_x2 < ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x)
				ImGui::SameLine();
		}
		if (ImGui::TreeNodeEx("Info", ImGuiTreeNodeFlags_DefaultOpen)) {
			Character& c = font->getCharacter(selectedCharacter);
			ImGui::Text("Character: %s", std::string(1, (char) selectedCharacter).c_str());
			ImGui::Text("ID: %d", c.id);
			ImGui::Text("Origin: (%d, %d)", c.x, c.x);
			ImGui::Text("Size: (%d, %d)", c.width, c.height);
			ImGui::Text("Bearing: (%d, %d)", c.bx, c.by);
			ImGui::Text("Advance: %d", c.advance);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

void renderPropertyEditor(ShaderResource* shader, Property& property) {
	ImGui::PushID(&property);
	std::string name(property.name);
	std::string uniform(property.uniform);

	auto renderVectorProperty = [&] (const char** labels, float* data, float** min, float** max) {
		for (std::size_t i = 0; i < property.defaults.size(); i++) {
			Property::Default& def = property.defaults.at(i);

			*(data + i) = def.value;
			*(min + i) = def.range.has_value() ? &def.range->min : nullptr;
			*(max + i) = def.range.has_value() ? &def.range->max : nullptr;
		}

		PROPERTY_DESC_IF(name.c_str(), uniform.c_str(), ImGui::DragVecN("", labels, data, property.defaults.size(), 0.0f, 0.1f, false, min, max),
			for (std::size_t i = 0; i < property.defaults.size(); i++) 
				property.defaults.at(i).value = data[i];
			);
	};
	
	switch (property.type) {
		case Property::Type::None: 
			break;
		case Property::Type::Bool: {
			Property::Default& def = property.defaults[0];
			bool value = def.value != 0.0f;
			PROPERTY_DESC_IF(name.c_str(), uniform.c_str(), ImGui::Checkbox("", &value),
				def.value = value ? 1.0f : 0.0f;
			);
				
			break;
		} case Property::Type::Int: {
			Property::Default& def = property.defaults[0];
			if (def.range.has_value()) {
				int value = static_cast<int>(def.value);
				PROPERTY_DESC_IF(name.c_str(), uniform.c_str(), ImGui::DragInt("", &value, 1.0f, static_cast<int>(def.range->min), static_cast<int>(def.range->max)),
					def.value = static_cast<float>(value);
				);
			} else {
				int value = static_cast<int>(def.value);
				PROPERTY_DESC_IF(name.c_str(), uniform.c_str(), ImGui::InputInt("", &value),
					def.value = static_cast<float>(value);
				);
			}
		} break;
		case Property::Type::Float: {
			Property::Default& def = property.defaults[0];
			if (def.range.has_value()) {
				PROPERTY_DESC(name.c_str(), uniform.c_str(), ImGui::DragFloat("", &def.value, 1.0f, def.range->min, def.range->max););
			} else {
				PROPERTY_DESC(name.c_str(), uniform.c_str(), ImGui::InputFloat("", &def.value));
			}
		} break;
		case Property::Type::Vec2: {
			float data[2], *min[2], *max[2];
			const char* labels[2] { "X", "Y" };
			renderVectorProperty(labels, data, min, max);
			
		} break;
		case Property::Type::Vec3: {
			float data[3], *min[3], *max[3];
			const char* labels[3] { "X", "Y", "Z" };
			renderVectorProperty(labels, data, min, max);
		} break;
		case Property::Type::Vec4: {
			float data[4], *min[4], *max[4];
			const char* labels[4] { "X", "Y", "Z", "W" };
			renderVectorProperty(labels, data, min, max);
		} break;
		case Property::Type::Mat2: break;
		case Property::Type::Mat3: break;
		case Property::Type::Mat4: break;
		case Property::Type::Col3: {
			float data[3], *min[3], *max[3];
			const char* labels[3] { "R", "G", "B" };
			renderVectorProperty(labels, data, min, max);
		} break;
		case Property::Type::Col4: {
			float data[4], *min[4], *max[4];
			const char* labels[4] { "R", "G", "B", "A" };
			renderVectorProperty(labels, data, min, max);
		} break;
	}
	
	ImGui::PopID();
}
	
void ResourceFrame::renderShaderInfo(ShaderResource* shader) {
	ImGui::BeginChild("Shaders");

	PROPERTY_FRAME_START("Properties");

	for (Property& property : shader->properties)
		renderPropertyEditor(shader, property);

	PROPERTY_FRAME_END;
	
	/*PROPERTY_FRAME_START("Code");
	if (ImGui::BeginTabBar("##tabs")) {
		for (const auto& [id, stage] : shader->stages) {
			if (ImGui::BeginTabItem(Shader::getStageName(id).c_str())) {
				std::string code(stage.view);
				ImGui::TextWrapped("%s", code.c_str());
			}
		}

		ImGui::EndTabBar();
	}
	PROPERTY_FRAME_END;*/
	
	ImGui::EndChild();
}

void ResourceFrame::onInit(Engine::Registry64& registry) {
	
}

void ResourceFrame::onRender(Engine::Registry64& registry) {
	ImGui::Begin("Resources");
	
	ImGui::Columns(2, 0, true);
	ImGui::Separator();

	auto map = ResourceManager::getResourceMap();
	for (auto& [name, resources] : map) {
		if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth)) {
			for (Resource* resource : resources) {
				if (ImGui::Selectable(resource->getName().c_str(), resource == selectedResource))
					selectedResource = resource;
			}
			ImGui::TreePop();
		}
	}

	ImGui::NextColumn();

	if (selectedResource) {
		ImGui::Text("Name: %s", selectedResource->getName().c_str());
		ImGui::Text("Path: %s", selectedResource->getPath().c_str());
		ImGui::Text("Type: %s", selectedResource->getTypeName().c_str());
		switch (selectedResource->getType()) {
			case ResourceType::Texture:
				renderTextureInfo(static_cast<TextureResource*>(selectedResource));
				break;
			case ResourceType::Font:
				renderFontInfo(static_cast<FontResource*>(selectedResource));
				break;
			case ResourceType::Shader:
				renderShaderInfo(static_cast<ShaderResource*>(selectedResource));
				break;
			default:
				ImGui::Text("Visual respresentation not supported.");
				break;
		}
	} else {
		ImGui::Text("No resource selected.");
	}

	ImGui::Columns(1);
	ImGui::Separator();
	
	ImGui::End();
}
	
}
