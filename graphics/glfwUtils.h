#pragma once

struct GLFWwindow;

namespace Graphics {

namespace GLFW {

namespace Cursor {
	extern int ARROW;
	extern int IBEAM;
	extern int CROSSHAIR;
	extern int HAND;
	extern int HRESIZE;
	extern int VRESIZE;
}

extern bool init();
extern void terminate();

extern GLFWwindow* getCurrentContext();
extern GLFWwindow* createContext(int width, int height, const char* title);
extern void setCurrentContext(GLFWwindow* context);
extern bool validContext(GLFWwindow* context);

extern void swapInterval(int interval);
extern void swapBuffers();
extern void pollEvents();

extern void closeWindow();
extern bool isWindowClosed();

extern Vec2i getWindowSize();
extern Vec4i getFrameSize();

extern void enableCursor();
extern void disableCursor();
extern void setCursor(int type);

extern void setMultisampleSamples(int samples);
}

};