#include "core.h"

#include "GL/glew.h"

#include "renderer.h"
#include <stack>

namespace P3D::Graphics {

namespace Renderer {

GLFLAG WIREFRAME = GL_LINE;
GLFLAG FILL = GL_FILL;
GLFLAG POINT = GL_POINT;
GLFLAG FRONT_AND_BACK = GL_FRONT_AND_BACK;

GLFLAG STATIC_DRAW = GL_STATIC_DRAW;
GLFLAG STREAM_DRAW = GL_STREAM_DRAW;
GLFLAG DYNAMIC_DRAW = GL_DYNAMIC_DRAW;

GLFLAG ARRAY_BUFFER = GL_ARRAY_BUFFER;

GLFLAG PATCHES = GL_PATCHES;
GLFLAG QUADS = GL_QUADS;
GLFLAG LINE_STRIP = GL_LINE_STRIP;
GLFLAG TRIANGLES = GL_TRIANGLES;
GLFLAG LINES = GL_LINES;
GLFLAG POINTS = GL_POINTS;

GLFLAG FLOAT = GL_FLOAT;
GLFLAG UINT = GL_UNSIGNED_INT;

std::stack<RenderState> states;
RenderState current;

RenderState saveState() {
	RenderState state;

	int intbuffer;
	unsigned char boolbuffer;
	/*glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &intbuffer);
	state.dfbo = intbuffer;
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &intbuffer);
	state.rfbo = intbuffer;
	glGetIntegerv(GL_RENDERBUFFER_BINDING, &intbuffer);
	state.rbo = intbuffer;*/
	// glGetIntegerv(GL_TEXTURE_BINDING_2D, &intbuffer);
	// state.texture = intbuffer;
	// glGetIntegerv(GL_CURRENT_PROGRAM, &intbuffer);
	// state.program = intbuffer;
	// glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &intbuffer);
	// state.vao = intbuffer;
	// glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &intbuffer);
	// state.vbo = intbuffer;
	// glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &intbuffer);
	// state.vbo = intbuffer;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &boolbuffer);
	state.depthmask = boolbuffer;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &state.activeTexture);
	glGetIntegerv(GL_VIEWPORT, state.viewport);
	glGetIntegerv(GL_POLYGON_MODE, state.mode);
	state.blend = glIsEnabled(GL_BLEND);
	state.cull = glIsEnabled(GL_CULL_FACE);
	state.depth = glIsEnabled(GL_DEPTH_TEST);
	//state.scissor = glIsEnabled(GL_SCISSOR_TEST);

	return state;
}

void loadState(const RenderState& state) {
	current = state;

	/*bindReadbuffer(state.rfbo);
	bindDrawbuffer(state.dfbo);
	bindRenderbuffer(state.rbo);*/
	// glUseProgram(state.program);
	// glBindVertexArray(state.vao);
	// glBindBuffer(GL_ARRAY_BUFFER, state.vbo);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state.ibo);
	// glActiveTexture(state.activeTexture);
	// glBindTexture(GL_TEXTURE_2D, state.texture);
	glDepthMask(state.depthmask); 
	glViewport(state.viewport[0], state.viewport[1], state.viewport[2], state.viewport[3]);
	glPolygonMode(state.mode[0], state.mode[1]);
	if (state.blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (state.cull) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (state.depth) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	//if (state.scissor) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
}

RenderState getState() {
	RenderState state;

	int intbuffer;
	unsigned char boolbuffer;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &intbuffer);
	state.dfbo = intbuffer;
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &intbuffer);
	state.rfbo = intbuffer;
	glGetIntegerv(GL_RENDERBUFFER_BINDING, &intbuffer);
	state.rbo = intbuffer;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &intbuffer);
	state.texture = intbuffer;
	glGetIntegerv(GL_CURRENT_PROGRAM, &intbuffer);
	state.program = intbuffer;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &intbuffer);
	state.vao = intbuffer;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &intbuffer);
	state.vbo = intbuffer;
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &intbuffer);
	state.vbo = intbuffer;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &boolbuffer);
	state.depthmask = boolbuffer;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &state.activeTexture);
	glGetIntegerv(GL_VIEWPORT, state.viewport);
	glGetIntegerv(GL_POLYGON_MODE, state.mode);
	state.blend = glIsEnabled(GL_BLEND);
	state.cull = glIsEnabled(GL_CULL_FACE);
	state.depth = glIsEnabled(GL_DEPTH_TEST);
	state.scissor = glIsEnabled(GL_SCISSOR_TEST);

	return state;
}

void beginScene() {
	RenderState	backup = saveState();
	states.push(backup);
}

void endScene() {
	RenderState backup = states.top();
	loadState(backup);
	states.pop();
}

bool initGLEW() {
	return glewInit() == GLEW_OK;
}

void clearDepth() {
	glClear(GL_DEPTH_BUFFER_BIT);
}

void clearColor() {
	glClear(GL_COLOR_BUFFER_BIT);
}

void clearStencil() {
	glClear(GL_STENCIL_BUFFER_BIT);
}

void lineStipple(int factor, short pattern) {
	glLineStipple(factor, pattern);
}

void lineWidth(float size) {
	glLineWidth(size);
}

void viewport(const Vec2i& origin, const Vec2i& dimension) {
	current.viewport[0] = origin.x;
	current.viewport[1] = origin.y;
	current.viewport[2] = dimension.x;
	current.viewport[3] = dimension.y;
	glViewport(origin.x, origin.y, dimension.x, dimension.y);
}

void enableDepthTest() {
	glEnable(GL_DEPTH_TEST);
}

void disableDepthTest() {
	glDisable(GL_DEPTH_TEST);
}

void enableDepthMask() {
	glDepthMask(GL_TRUE);
}

void disableDepthMask() {
	glDepthMask(GL_FALSE);
}

void enableCulling() {
	glEnable(GL_CULL_FACE);
}

void disableCulling() {
	glDisable(GL_CULL_FACE);
}

void enableBlending() {
	glEnable(GL_BLEND);
}

void disableBlending() {
	glDisable(GL_BLEND);
}

void standardBlendFunction() {
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void enableMultisampling() {
	glEnable(GL_MULTISAMPLE);
}

void polygonMode(int face, int mode) {
	glPolygonMode(face, mode);
}

const char* getVendor() {
	return reinterpret_cast<const char*>(glGetString(GL_VENDOR));
}

const char* getVersion() {
	return reinterpret_cast<const char*>(glGetString(GL_VERSION));
}

const char* getRenderer() {
	return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
}

const char* getShaderVersion() {
	return reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
}

int parseShaderVersion(const char* version) {
	return 100 * (version[0] - '0') + 10 * (version[2] - '0');
}

void genBuffers(int count, GLID* id) {
	glGenBuffers(count, id);
}

void genVertexArrays(int count, GLID* id) {
	glGenVertexArrays(count, id);
}

void bindBuffer(GLFLAG target, GLID id) {
	glBindBuffer(target, id);
}

void bufferData(GLFLAG target, int size, int offset, GLFLAG type) {
	glBufferData(target, size, reinterpret_cast<const void*>(offset), type);
}

void bufferSubData(GLFLAG target, int offset, int size, const void* pointer) {
	glBufferSubData(target, offset, size, pointer);
}

void enableVertexAttribArray(GLID id) {
	glEnableVertexAttribArray(id);
}

void bindTexture2D(GLID id) {
	glBindTexture(GL_TEXTURE_2D, id);
}

void activeTexture(GLID unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
}

void vertexAttribPointer(GLID id, int size, GLFLAG type, bool normalized, int stride, const void* pointer) {
	glVertexAttribPointer(id, size, type, normalized, stride, pointer);
}

void bindShader(GLID id) {
	//if (current.program != id) {
	//	current.program = id;
		glUseProgram(id);
	//}
}

void bindFramebuffer(GLID id) {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void bindDrawbuffer(GLID id) {
	//if (current.dfbo != id) {
	//	current.dfbo = id;
		glBindFramebuffer(GL_DRAW_BUFFER, id);
	//}
}

void bindReadbuffer(GLID id) {
	//if (current.rfbo != id) {
	//	current.rfbo = id;
		glBindFramebuffer(GL_READ_BUFFER, id);
	//}
}

void bindRenderbuffer(GLID id) {
	//if (current.rbo != id) {
	//	current.rbo = id;
		glBindRenderbuffer(GL_RENDERBUFFER, id);
	//}
}

void bindVertexArray(GLID id) {
	glBindVertexArray(id);
}

void scissor(int x, int y, int width, int height) {
	glScissor(x, y, width, height);
}

void drawElements(GLFLAG mode, std::size_t count, GLFLAG type, const void* offset) {
	glDrawElements(mode, count, type, offset);
}

void drawElementsInstanced(GLFLAG mode, std::size_t count, GLFLAG type, const void* offset, std::size_t primitives) {
	glDrawElementsInstanced(mode, count, type, offset, primitives);
}

void drawArrays(GLFLAG mode, int first, std::size_t count) {
	glDrawArrays(mode, first, count);
}

void defaultSettings(GLID defaultFrameBuffer) {
	bindFramebuffer(defaultFrameBuffer);
	standardBlendFunction();
	enableDepthTest();
	glLineWidth(1.5);
	disableCulling();
	clearColor();
	clearDepth();
}

}

};