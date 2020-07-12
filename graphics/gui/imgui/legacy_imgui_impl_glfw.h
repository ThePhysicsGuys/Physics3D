#pragma once

struct GLFWwindow;

bool ImGuiInitGLFW(GLFWwindow* window, bool install_callbacks);
void ImGuiShutdownGLFW();
void ImGuiNewFrameGLFW();
void ImGuiMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void ImGuiScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void ImGuiKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void ImGuiCharCallback(GLFWwindow* window, unsigned int c);
