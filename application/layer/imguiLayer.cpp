#include "core.h"

#include "GL/glew.h"
#include "imguiLayer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

#include "../view/screen.h"
#include "../graphics/glfwUtils.h"
#include "../graphics/texture.h"
#include "../graphics/buffers/frameBuffer.h"
#include "../graphics/renderer.h"
#include "../util/resource/resourceManager.h"
#include "../graphics/resource/textureResource.h"
#include "../engine/event/windowEvent.h"
#include "../application/input/standardInputHandler.h"
#include "../graphics/gui/gui.h"
#include "../physics/misc/toString.h"

namespace P3D::Application {

void ImGuiLayer::onInit(Engine::Registry64& registry) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void) io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	GLFWwindow* window = Graphics::GLFW::getCurrentContext();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void ImGuiLayer::onUpdate(Engine::Registry64& registry) {

}

void ImGuiLayer::onEvent(Engine::Registry64& registry, Engine::Event& event) {
	if (!sceneHovered) {
		event.handled = event.inCategory(Engine::EventCategoryMouse) && ImGui::GetIO().WantCaptureMouse;
	}
}

void ImGuiLayer::onRender(Engine::Registry64& registry) {
	Screen* screen = static_cast<Screen*>(this->ptr);

	ImGui::Begin("Scene", (bool*)0, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar /*| ImGuiWindowFlags_NoInputs*/);
	sceneHovered = ImGui::IsWindowHovered();
	ImVec2 pos = ImGui::GetWindowPos();
	ImVec2 min = ImGui::GetWindowContentRegionMin();
	ImVec2 max = ImGui::GetWindowContentRegionMax();
	
	
	//Log::debug("%f, %f,	     %f, %f,	     %f, %f", handler->mousePosition.x, handler->mousePosition.y, min.x, min.y, ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
	ImVec2 size = ImVec2(max.x - min.x, max.y - min.y);
	min = ImVec2(min.x + pos.x, min.y + pos.y);
	max = ImVec2(max.x + pos.x, max.y + pos.y);
	ImGui::GetForegroundDrawList()->AddRect(min, max, IM_COL32(255, 255, 0, 255));
	handler->viewport = Vec4(min.x, min.y, size.x, size.y);

	Graphics::Texture* texture = screen->screenFrameBuffer->texture;
	ImGui::Image((void*) texture->getID(), size, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();

	if (Graphics::GUI::windowInfo.dimension.x != size.x || Graphics::GUI::windowInfo.dimension.y != size.y) {
		Engine::FrameBufferResizeEvent event(size.x, size.y);
		handler->onFrameBufferResize(event);
	}
}

void ImGuiLayer::onClose(Engine::Registry64& registry) {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiLayer::begin() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	if (opt_fullscreen) {
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Physics3D", (bool*) true, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
		ImGuiID dockspace_id = ImGui::GetID("DockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save world", "CTRL-S"));
			if (ImGui::MenuItem("Load world", "CTRL-O"));
			if (ImGui::MenuItem("Exit", "esc"));

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Play / pause", "P"));
			if (ImGui::MenuItem("Tick", "T"));
			if (ImGui::MenuItem("New part", "O"));
			if (ImGui::MenuItem("Delete part", "DELETE"));

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Debug")) {
			if (ImGui::MenuItem("Debug info", "F"));

			ImGui::EndMenu();
		}

		
		ImGui::EndMenuBar();
	}
}

void ImGuiLayer::end() {
	// End the dockspace
	ImGui::End();

	// End imgui
	ImGuiIO& io = ImGui::GetIO();
	Screen* screen = static_cast<Screen*>(this->ptr);
	io.DisplaySize = ImVec2((float) screen->dimension.x, (float) screen->dimension.y);
	ImGui::Render();
	
	struct {
		int draw;
		int read;
		int render;
	} info;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &info.draw);
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &info.read);
	glGetIntegerv(GL_RENDERBUFFER_BINDING, &info.render);

	Graphics::Renderer::bindFramebuffer(0);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		GLFWwindow* backupContext = Graphics::GLFW::getCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		Graphics::GLFW::setCurrentContext(backupContext);
	}
}

};
