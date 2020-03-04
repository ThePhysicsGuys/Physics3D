#include "core.h"

#include "GL/glew.h"

#include "renderer.h"
#include <stack>

namespace Renderer {

unsigned int WIREFRAME = GL_LINE;
unsigned int FILL = GL_FILL;
unsigned int POINT = GL_POINT;
unsigned int FRONT_AND_BACK = GL_FRONT_AND_BACK;

unsigned int STATIC_DRAW = GL_STATIC_DRAW;
unsigned int STREAM_DRAW = GL_STREAM_DRAW;
unsigned int DYNAMIC_DRAW = GL_DYNAMIC_DRAW;

unsigned int PATCHES = GL_PATCHES;
unsigned int QUADS = GL_QUADS;
unsigned int TRIANGLES = GL_TRIANGLES;
unsigned int LINES = GL_LINES;
unsigned int POINTS = GL_POINTS;

unsigned int UINT = GL_UNSIGNED_INT;

struct RenderState {
	GLID dfbo;
	GLID rfbo;
	GLID rbo;
	GLID texture; 
	//GLID program; 
	GLID vao; 
	GLID vbo; 
	GLID ibo; 
	int activeTexture; 
	int viewport[4]; 
	int mode[2];  
	bool blend;   
	bool cull;	  
	bool depth;   
	bool scissor; 
	bool depthmask;
};

std::stack<RenderState> states;
RenderState current;

RenderState saveState() {
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
	// glGetIntegerv(GL_CURRENT_PROGRAM, &intbuffer);
	// state.program = intbuffer;
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

void loadState(const RenderState& state) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, state.rfbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, state.dfbo);
	glBindRenderbuffer(GL_RENDERBUFFER, state.rbo);
	glBindTexture(GL_TEXTURE_2D, state.texture);
	// glUseProgram(state.program);
	// glBindVertexArray(state.vao);
	// glBindBuffer(GL_ARRAY_BUFFER, state.vbo);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state.ibo);
	glActiveTexture(state.activeTexture);
	glViewport(state.viewport[0], state.viewport[1], state.viewport[2], state.viewport[3]);
	glPolygonMode(state.mode[0], state.mode[1]);
	glDepthMask(state.depthmask);
	if (state.blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (state.cull) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (state.depth) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (state.scissor) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
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

void viewport(Vec2i origin, Vec2i dimension) {
	current.viewport[0] = origin.x;
	current.viewport[1] = origin.y;
	current.viewport[2] = dimension.x;
	current.viewport[3] = dimension.y;
	glViewport(origin.x, origin.y, dimension.x, dimension.y);
}

inline void enableDepthTest() {
	current.depth = true;
	glEnable(GL_DEPTH_TEST);
}

inline void disableDepthTest() {
	current.depth = false;
	glDisable(GL_DEPTH_TEST);
}

inline void enableDepthMask() {
	current.depthmask = true;
	glDepthMask(GL_TRUE);
}

inline void disableDepthMask() {
	current.depthmask = false;
	glDepthMask(GL_FALSE);
}

inline void enableCulling() {
	current.cull = true;
	glEnable(GL_CULL_FACE);
}

inline void disableCulling() {
	current.cull = false;
	glDisable(GL_CULL_FACE);
}

inline void enableBlending() {
	current.blend = true;
	glEnable(GL_BLEND);
}

inline void disableBlending() {
	current.blend = false;
	glDisable(GL_BLEND);
}

inline void standardBlendFunction() {
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

inline void enableMultisampling() {
	glEnable(GL_MULTISAMPLE);
}

void polygonMode(int face, int mode) {
	current.mode[0] = face;
	current.mode[1] = mode;
	glPolygonMode(face, mode);
}

const unsigned char* getVendor() {
	return glGetString(GL_VENDOR);
}

const unsigned char* getVersion() {
	return glGetString(GL_VERSION);
}

const unsigned char* getRenderer() {
	return glGetString(GL_RENDERER);
}

const unsigned char* getShaderVersion() {
	return glGetString(GL_SHADING_LANGUAGE_VERSION);
}

void bindTexture2D(int id) {
	glBindTexture(GL_TEXTURE_2D, id);
}

void scissor(int x, int y, int width, int height) {
	glScissor(x, y, width, height);
}

void drawElements(unsigned int mode, size_t count, unsigned int type, const void* offset) {
	glDrawElements(mode, count, type, offset);
}

void drawArrays(unsigned int mode, int first, size_t count) {
	glDrawArrays(mode, first, count);
}

}