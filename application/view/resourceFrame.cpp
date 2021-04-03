#include "core.h"

#include "resourceFrame.h"

#include "imgui/imgui.h"
#include "shader/shaders.h"
#include "../util/resource/resourceManager.h"
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

/*
void renderShaderTypeEditor(GShader* shader, const Parser::Local& uniform) {
	Parser::Type type = Parser::parseType(uniform.type);
	std::string name(uniform.name);
	switch (type) {
		case Parser::Type::None:
		case Parser::Type::Void:
		case Parser::Type::Struct:
		case Parser::Type::VSOut:
		case Parser::Type::Mat2:
		case Parser::Type::Mat3:
		case Parser::Type::Mat4:
			ImGui::Text("No editor available");
			ImGui::Text("No editor available");
			break;
		case Parser::Type::Int: {
				int value = 0;
				if (ImGui::InputInt(name.c_str(), &value))
					shader->setUniform(name, value);
			} break;
		case Parser::Type::Float: {
				float value = 0.0f;
				if (ImGui::InputFloat(name.c_str(), &value))
					shader->setUniform(name, value);
			} break;
		case Parser::Type::Vec2: {
				Vec2f value = Vec2f(0, 0);
				if (ImGui::InputFloat2(name.c_str(), value.data))
					shader->setUniform(name, value);
			} break;
		case Parser::Type::Vec3: {
				Vec3f value = Vec3f(0, 0, 0);
				if (ImGui::InputFloat3(name.c_str(), value.data))
					shader->setUniform(name, value);
			} break;
		case Parser::Type::Vec4: {
				Vec4f value = Vec4f(0, 0, 0, 0);
				if (ImGui::InputFloat4(name.c_str(), value.data))
					shader->setUniform(name, value);
			} break;
		case Parser::Type::Sampler2D: {
				int value = 0;
				if (ImGui::InputInt(name.c_str(), &value))
					shader->setUniform(name, value);
			} break;
		case Parser::Type::Sampler3D: {
				int value = 0;
				if (ImGui::InputInt(name.c_str(), &value))
					shader->setUniform(name, value);
			} break;
		default:
			break;
	}
}
*/
	
void ResourceFrame::renderShaderStageInfo(ShaderResource* shader, const ShaderStage& stage) {
	if (ImGui::TreeNode("Code")) {
		ImGui::TextWrapped("%s", stage.source.c_str());

		ImGui::TreePop();
	}

	if (stage.info.uniforms.size()) {
		if (ImGui::TreeNode("Uniforms")) {
			ImGui::Columns(5);
			ImGui::Separator();
			ImGui::Text("ID"); ImGui::NextColumn();
			ImGui::Text("Name"); ImGui::NextColumn();
			ImGui::Text("Type"); ImGui::NextColumn();
			ImGui::Text("Array"); ImGui::NextColumn();
			ImGui::Text("Edit"); ImGui::NextColumn();
			ImGui::Separator();

			for (const Parser::Local& uniform : stage.info.uniforms) {
				std::string name(uniform.name.view(stage.source.c_str()));
				std::string type(uniform.type.view(stage.source.c_str()));
				ImGui::Text("%d", shader->getUniform(name)); ImGui::NextColumn();
				ImGui::Text("%s", name.c_str()); ImGui::NextColumn();
				ImGui::Text("%s", type.c_str()); ImGui::NextColumn();
				if (uniform.amount != 0) ImGui::Text("yes: %d", uniform.amount); else ImGui::Text("no"); ImGui::NextColumn();
				ImGui::Text("No editor available"); ImGui::NextColumn();
				//if (!uniform.array) renderShaderTypeEditor(shader, uniform); else ImGui::Text("No editor available"); ImGui::NextColumn();
			}
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::TreePop();
		}
	}

	if (stage.info.locals.size()) {
		if (ImGui::TreeNode("Locals")) {
			ImGui::Columns(3);
			ImGui::Separator();
			ImGui::Text("Name"); ImGui::NextColumn();
			ImGui::Text("Type"); ImGui::NextColumn();
			ImGui::Text("Array"); ImGui::NextColumn();
			ImGui::Separator();

			for (const Parser::Local& local : stage.info.locals) {
				std::string name(local.name.view(stage.source.c_str()));
				std::string type(local.type.view(stage.source.c_str()));
				ImGui::Text("%s", name.c_str()); ImGui::NextColumn();
				ImGui::Text("%s", type.c_str()); ImGui::NextColumn();
				if (local.amount != 0)
					ImGui::Text("yes: %d", local.amount);
				else
					ImGui::Text("no");

				ImGui::NextColumn();
			}
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::TreePop();
		}
	}

	if (stage.info.globals.size()) {
		if (ImGui::TreeNode("Globals")) {
			ImGui::Columns(4);
			ImGui::Separator();
			ImGui::Text("Name"); ImGui::NextColumn();
			ImGui::Text("IO"); ImGui::NextColumn();
			ImGui::Text("Type"); ImGui::NextColumn();
			ImGui::Text("Array"); ImGui::NextColumn();
			ImGui::Separator();

			for (const Parser::Global& global : stage.info.globals) {
				std::string name(global.name.view(stage.source.c_str()));
				std::string type(global.type.view(stage.source.c_str()));
				ImGui::Text("%s", name.c_str()); ImGui::NextColumn();
				ImGui::Text("%s", global.io == Parser::IO::In ? "in" : "out"); ImGui::NextColumn();
				ImGui::Text("%s", type.c_str()); ImGui::NextColumn();
				if (global.amount != 0)
					ImGui::Text("yes: %d", global.amount);
				else
					ImGui::Text("no");

				ImGui::NextColumn();
			}
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::TreePop();
		}
	}

	if (stage.info.structs.size()) {
		if (ImGui::TreeNode("Structs")) {
			for (const auto& strctit : stage.info.structs) {
				const Parser::Struct& strct = strctit.second;

				if (ImGui::TreeNode(strct.name.string(stage.source.c_str()).c_str())) {
					ImGui::Columns(3);
					ImGui::Separator();
					ImGui::Text("Name"); ImGui::NextColumn();
					ImGui::Text("Type"); ImGui::NextColumn();
					ImGui::Text("Array"); ImGui::NextColumn();
					ImGui::Separator();

					for (const Parser::Local& local : strct.locals) {
						std::string name(local.name.view(stage.source.c_str()));
						std::string type(local.type.view(stage.source.c_str()));
						ImGui::Text("%s", name.c_str()); ImGui::NextColumn();
						ImGui::Text("%s", type.c_str()); ImGui::NextColumn();
						if (local.amount != 0)
							ImGui::Text("yes: %d", local.amount);
						else
							ImGui::Text("no");

						ImGui::NextColumn();
					}
					ImGui::Columns(1);
					ImGui::Separator();
					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}
	}

	if (stage.info.defines.size()) {
		if (ImGui::TreeNode("Defines")) {
			ImGui::Columns(2);
			ImGui::Separator();
			ImGui::Text("Name"); ImGui::NextColumn();
			ImGui::Text("Value"); ImGui::NextColumn();
			ImGui::Separator();

			for (const auto& define : stage.info.defines) {
				ImGui::Text("%s", std::string(define.first).c_str()); ImGui::NextColumn();
				ImGui::Text("%d", (int) define.second); ImGui::NextColumn();
			}
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::TreePop();
		}
	}
}
	
void ResourceFrame::renderShaderInfo(ShaderResource* shader) {
	ImGui::BeginChild("Shaders");

	if (ImGui::BeginTabBar("##tabs")) {
		if (shader->flags & shader->VERTEX) {
			if (ImGui::BeginTabItem("Vertex")) {
				renderShaderStageInfo(shader, shader->vertexStage);
				ImGui::EndTabItem();
			}
		}
		if (shader->flags & shader->GEOMETRY) {
			if (ImGui::BeginTabItem("Geometry")) {
				renderShaderStageInfo(shader, shader->geometryStage);
				ImGui::EndTabItem();
			}
		}
		if (shader->flags & shader->TESSELATION_CONTROL) {
			if (ImGui::BeginTabItem("Tesselation control")) {
				renderShaderStageInfo(shader, shader->tesselationControlStage);
				ImGui::EndTabItem();
			}
		}
		if (shader->flags & shader->TESSELATION_EVALUATE) {
			if (ImGui::BeginTabItem("Tesselation evaluate")) {
				renderShaderStageInfo(shader, shader->tesselationEvaluationStage);
				ImGui::EndTabItem();
			}
		}
		if (shader->flags & shader->FRAGMENT) {
			if (ImGui::BeginTabItem("Fragment")) {
				renderShaderStageInfo(shader, shader->fragmentStage);
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}

	if (ImGui::Button("Reload")) {
		ShaderSource shaderSource = parseShader(shader->getName(), shader->getPath());
		if (shaderSource.vertexSource.empty() || shaderSource.fragmentSource.empty()) {
			ImGui::EndChild();
			return;
		}
		shader->reload(shaderSource);
	}

	ImGui::EndChild();
}

void ResourceFrame::onInit(Engine::Registry64& registry) {
	
}

void ResourceFrame::onRender(Engine::Registry64& registry) {
	ImGui::Begin("Resources");
	
	ImGui::Columns(2, 0, true);
	ImGui::Separator();

	auto map = ResourceManager::getResourceMap();
	for (auto iterator = map.begin(); iterator != map.end(); ++iterator) {
		if (ImGui::TreeNodeEx(iterator->first.c_str(), ImGuiTreeNodeFlags_SpanFullWidth)) {
			for (Resource* resource : iterator->second) {
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
			case ResourceType::GShader:
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
