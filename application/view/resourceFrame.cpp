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
		renderTextureInfo(font->getAtlas().get());
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
			ImGui::Text("Character: %s", std::string(1, static_cast<char>(selectedCharacter)).c_str());
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

	auto renderVectorProperty = [&] (const char** labels, float* data, float* speed, float** min, float** max) -> bool {
		for (std::size_t i = 0; i < property.defaults.size(); i++) {
			Property::Default& def = property.defaults.at(i);
			*(data + i) = def.value;
			*(speed + i) = def.step.value_or(0.1f);
			*(min + i) = def.range.has_value() ? &def.range->min : nullptr;
			*(max + i) = def.range.has_value() ? &def.range->max : nullptr;
		}

		PROPERTY_DESC_IF(name.c_str(), uniform.c_str(), ImGui::DragVecN("", labels, data, property.defaults.size(), 0.0f, speed, false, min, max),
			for (std::size_t i = 0; i < property.defaults.size(); i++) 
				property.defaults.at(i).value = data[i];
			return true;
		);

		return false;
	};

	auto renderMatrixProperty = [&] (const char** labels, float* data, float* speed, float** min, float** max) -> bool {	
		for (std::size_t i = 0; i < property.defaults.size(); i++) {
			Property::Default& def = property.defaults.at(i);
			*(data + i) = def.value;
			*(speed + i) = def.step.value_or(0.1f);
			*(min + i) = def.range.has_value() ? &def.range->min : nullptr;
			*(max + i) = def.range.has_value() ? &def.range->max : nullptr;
		}

		std::size_t size = 2;
		if (property.defaults.size() == 9)
			size = 3;
		else if (property.defaults.size() == 16)
			size = 4;
		
		bool result = false;
		PROPERTY_DESC_IF(name.c_str(), uniform.c_str(), ImGui::DragVecN("", labels, data, size, 0.0f, speed, false, min, max),
			for (std::size_t i = 0; i < property.defaults.size(); i++)
				property.defaults.at(i).value = data[i];

			result = true;
		);
		
		for (std::size_t j = 1; j < size; j++) {
			std::size_t offset = j * size;
			PROPERTY_IF("", ImGui::DragVecN("", labels + offset, data + offset, size, 0.0f, speed, false, min + offset, max + offset),
				for (std::size_t i = 0; i < property.defaults.size(); i++)
					property.defaults.at(i).value = data[i];
			);

			result = true;
		}

		return result;
	};
	
	switch (property.type) {
		case Property::Type::None: 
			break;
		case Property::Type::Bool: {
			Property::Default& def = property.defaults[0];
			bool value = def.value != 0.0f;
			PROPERTY_DESC_IF(name.c_str(), uniform.c_str(), ImGui::Checkbox("", &value),
				def.value = value ? 1.0f : 0.0f;
				shader->bind();
				shader->setUniform(uniform, value);
			);
				
			break;
		} case Property::Type::Int: {
			Property::Default& def = property.defaults[0];
			if (def.range.has_value()) {
				int value = static_cast<int>(def.value);
				PROPERTY_DESC_IF(name.c_str(), uniform.c_str(), ImGui::DragInt("", &value, 1.0f, static_cast<int>(def.range->min), static_cast<int>(def.range->max)),
					def.value = static_cast<float>(value);
					shader->bind();
					shader->setUniform(uniform, value);
				);
			} else {
				int value = static_cast<int>(def.value);
				PROPERTY_DESC_IF(name.c_str(), uniform.c_str(), ImGui::InputInt("", &value),
					def.value = static_cast<float>(value);
					shader->bind();
					shader->setUniform(uniform, value);
				);
			}
		} break;
		case Property::Type::Float: {
			Property::Default& def = property.defaults[0];
			if (def.range.has_value()) {
				PROPERTY_DESC_IF(name.c_str(), uniform.c_str(), ImGui::DragFloat("", &def.value, 1.0f, def.range->min, def.range->max),
					shader->bind();
					shader->setUniform(uniform, def.value);
				);
			} else {
				PROPERTY_DESC_IF(name.c_str(), uniform.c_str(), ImGui::InputFloat("", &def.value),
					shader->bind();
					shader->setUniform(uniform, def.value);
				);
			}
		} break;
		case Property::Type::Vec2: {
			float data[2], speed[2], *min[2], *max[2];
			const char* labels[2] { "X", "Y" };
			renderVectorProperty(labels, data, speed, min, max);
			shader->bind();
			shader->setUniform(uniform, Vec2f { data[0], data[1] });
		} break;
		case Property::Type::Vec3: {
			float data[3], speed[3], *min[3], *max[3];
			const char* labels[3] { "X", "Y", "Z" };
			renderVectorProperty(labels, data, speed, min, max);
			shader->bind();
			shader->setUniform(uniform, Vec3f { data[0], data[1], data[2] });
		} break;
		case Property::Type::Vec4: {
			float data[4], speed[4], *min[4], *max[4];
			const char* labels[4] { "X", "Y", "Z", "W" };
			renderVectorProperty(labels, data, speed, min, max);
			shader->bind();
			shader->setUniform(uniform, Vec4f { data[0], data[1], data[2], data[3] });
		} break;
		case Property::Type::Mat2: {
			float data[4], speed[4], *min[4], *max[4];
			const char* labels[4] { "00", "01", "10", "11" };
			renderMatrixProperty(labels, data, speed, min, max);
			shader->bind();
			shader->setUniform(uniform, Mat2f {
				data[0], data[1], 
				data[2], data[3] });
		} break;
		case Property::Type::Mat3: {
			float data[9], speed[9], *min[9], *max[9];
			const char* labels[9] { "00", "01", "02", "10", "11", "12", "20", "21", "22" };
			renderMatrixProperty(labels, data, speed, min, max);
			shader->bind();
			shader->setUniform(uniform, Mat3f { 
				data[0], data[1], data[2],
				data[3], data[4], data[5],
				data[6], data[7], data[8] });
		} break;
		case Property::Type::Mat4: {
			float data[16], speed[16], *min[16], *max[16];
			const char* labels[16] { "00", "01", "02", "03", "10", "11", "12", "13", "20", "21", "22", "23", "30", "31", "32", "33" };
			renderMatrixProperty(labels, data, speed, min, max);
			shader->bind();
			shader->setUniform(uniform, Mat3f {
				data[0], data[1], data[2], data[3],
				data[4], data[5], data[6], data[7],
				data[8], data[9], data[10], data[11],
				data[12], data[13], data[14], data[15] });
		} break;
		case Property::Type::Col3: {
			float data[3], speed[3], *min[3], *max[3];
			const char* labels[3] { "R", "G", "B" };
			renderVectorProperty(labels, data, speed, min, max);
			shader->bind();
			shader->setUniform(uniform, Vec3f { data[0], data[1], data[2] });
		} break;
		case Property::Type::Col4: {
			float data[4], speed[4], *min[4], *max[4];
			const char* labels[4] { "R", "G", "B", "A" };
			renderVectorProperty(labels, data, speed, min, max);
			shader->bind();
			shader->setUniform(uniform, Vec4f { data[0], data[1], data[2], data[3] });
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
	const char* colors[] = {
		"B12829FF", // Red Idle
		"611C1DFF", // Red Hover
		"852828FF", // Red Active

		"269032FF", // Green Idle
		"1C6124FF", // Green Hover
		"288533FF", // Green Active

		"2847B1FF", // Blue Idle
		"1C2C61FF", // Blue Hover
		"283E85FF", // Blue Active
	};
#define IM_F32_TO_INT8_SAT(_VAL)        ((int)(ImSaturate(_VAL) * 255.0f + 0.5f))               // Saturated, always 
	for (int id = 0; id < 9; id++) {
		int i[4];
		sscanf(colors[id], "%02X%02X%02X%02X", (unsigned int*) &i[0], (unsigned int*) &i[1], (unsigned int*) &i[2], (unsigned int*) &i[3]);
		ImVec4 col = {
			i[0] / 255.0f,
			i[1] / 255.0f,
			i[2] / 255.0f,
			i[3] / 255.0f,
		};
		ImU32 c = ImGui::ColorConvertFloat4ToU32(col);
		Log::debug("%u",c);
	}
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
