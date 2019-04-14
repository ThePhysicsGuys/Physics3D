#include "screen.h"

#include "shader.h"
#include "indexedMesh.h"
#include "arrayMesh.h"
#include "vectorMesh.h"
#include "picker.h"
#include "material.h"
#include "quad.h"
#include "shaderProgram.h"
#include "font.h"
#include "profilerUI.h"
#include "loader.h"

#include "form\panel.h"
#include "form\label.h"
#include "form\gui.h"

#include "../debug.h"
#include "../standardInputHandler.h"
#include "../resourceManager.h"
#include "../objectLibrary.h"

#include "../../util/log.h"

#include "../engine/math/vec2.h"
#include "../engine/math/mat4.h"
#include "../engine/math/mathUtil.h"
#include "../engine/geometry/shape.h"
#include "../engine/geometry/shape.h"
#include "../engine/physicsProfiler.h"
#include "../engine/geometry/boundingBox.h"
#include "../engine/debug.h"

#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <math.h>
#include <map>

#include "../engine/sharedLockGuard.h"

bool initGLFW() {
	/* Initialize GLFW */
	if (!glfwInit()) {
		Log::error("GLFW failed to initialize");
		return false;
	}

	Log::info("GLFW initialized");
	return true;
}

bool initGLEW() {
	// Init GLEW after creating a valid rendering context
	if (glewInit() != GLEW_OK) {
		glfwTerminate();

		Log::error("GLEW Failed to initialize!");

		return false;
	}

	Log::info("GLEW initialized");
	return true;
}

void terminateGL() {
	glfwTerminate();
}

const char* const graphicsDebugLabels[]{
	"Update",
	"Skybox",
	"Vectors",
	"Physicals",
	"Lighting",
	"Origin",
	"Profiler",
	"Finalize",
	"Other"
};

Screen::Screen() : graphicsMeasure(graphicsDebugLabels) {};
Screen::Screen(int width, int height, World* world) : graphicsMeasure(graphicsDebugLabels) {
	setWorld(world);

	// Create a windowed mode window and its OpenGL context 
	this->window = glfwCreateWindow(width, height, "Physics3D", NULL, NULL);
	
	if (!this->window) {
		glfwTerminate();
		exit(-1);
	}

	// Make the window's context current 
	glfwMakeContextCurrent(this->window);

	Log::info("OpenGL vendor: (%s)", glGetString(GL_VENDOR));
	Log::info("OpenGL renderer: (%s)", glGetString(GL_RENDERER));
	Log::info("OpenGL version: (%s)", glGetString(GL_VERSION));
	Log::info("OpenGL shader version: (%s)", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

// Debug
using namespace Debug;
std::map<VecType, bool> debug_enabled{ {INFO, true}, {VELOCITY, true}, {FORCE, true}, {POSITION, true}, {MOMENT, true}, {IMPULSE, true}, {ANGULAR_VELOCITY , true} };
std::map<VecType, double> vecColors{ {INFO, 0.15}, {VELOCITY, 0.3}, {FORCE, 0.0}, {POSITION, 0.5}, {MOMENT, 0.1}, {IMPULSE, 0.7}, {ANGULAR_VELOCITY , 0.75} };
bool renderPies = false;

// Font
Font* font = nullptr;

// Handler
StandardInputHandler* handler = nullptr;

// Textures
Texture* floorTexture = nullptr;

//Skybox
IndexedMesh* sphere = nullptr;
BoundingBox* skybox = nullptr;
CubeMap* skyboxTexture = nullptr;
IndexedMesh* skyboxMesh = nullptr;

// Render uniforms
Mat4f projectionMatrix;
Mat4f orthoMatrix;
Mat4f rotatedViewMatrix;
Mat4f viewMatrix;
Vec3f viewPosition;

// Shaders
BasicShader basicShader;
BasicNormalShader basicNormalShader;
VectorShader vectorShader;
OriginShader originShader;
FontShader fontShader;
QuadShader quadShader;
GUIShader guiShader;
PostProcessShader postProcessShader;
SkyboxShader skyboxShader;

// Object uniforms
Material material = Material (
	Vec3f(1, 1, 1),
	Vec3f(1, 1, 1),
	Vec3f(1, 1, 1),
	1
);

// Light uniforms
const int lightCount = 4;
Vec3f sunDirection;
Attenuation attenuation = { 0, 0, 0.5 };
Light lights[lightCount] = {
	Light(Vec3f(5, 0, 0), Vec3f(1, 1, 1), 2, attenuation),
	Light(Vec3f(0, 5, 0), Vec3f(1, 1, 1), 2, attenuation),
	Light(Vec3f(0, 0, 5), Vec3f(1, 1, 1), 2, attenuation),
	Light(Vec3f(0, 0, 0), Vec3f(1, 1, 1), 2, attenuation)
};

// Render meshes
VectorMesh* vectorMesh = nullptr;
ArrayMesh* originMesh = nullptr;
Quad* quad = nullptr;

// GUI
Panel* panel = nullptr;
Label* label1 = nullptr;
Label* label2 = nullptr;
Label* label3 = nullptr;

void Screen::init() {
	// Log init
	Log::setLogLevel(Log::Level::INFO);


	// Render mode init
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);


	// Screen size init
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	screenSize = Vec2(width, height);
	aspect = width / height;


	// Shader source init
	ShaderSource basicShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(BASIC_SHADER)), "basic.shader");
	ShaderSource basicNormalShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(BASICNORMAL_SHADER)), "basicnormal.shader");
	ShaderSource vectorShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(VECTOR_SHADER)), "vector.shader");
	ShaderSource fontShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(FONT_SHADER)), "font.shader");
	ShaderSource originShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(ORIGIN_SHADER)), "origin.shader");
	ShaderSource quadShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(QUAD_SHADER)), "quad.shader");
	ShaderSource postProcessShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(POSTPROCESS_SHADER)), "postProcess.shader");
	ShaderSource skyboxShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(SKYBOX_SHADER)), "skybox.shader");
	ShaderSource guiShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(GUI_SHADER)), "gui.shader");


	// Shader init
	basicShader = * new BasicShader(basicShaderSource);
	basicNormalShader = * new BasicNormalShader(basicNormalShaderSource);
	vectorShader = * new VectorShader(vectorShaderSource);
	fontShader = * new FontShader(fontShaderSource);
	originShader = * new OriginShader(originShaderSource);
	quadShader = * new QuadShader(quadShaderSource);
	postProcessShader = * new PostProcessShader(postProcessShaderSource);
	skyboxShader = * new SkyboxShader(skyboxShaderSource);
	guiShader = * new GUIShader(guiShaderSource);
	basicShader.createLightArray(lightCount);


	// Texture init
	floorTexture = load("../res/textures/floor/floor_color.jpg");
	material.setTexture(floorTexture);

	// Skybox init
	sphere = new IndexedMesh(loadMesh((std::istream&) std::istringstream(getResourceAsString(SPHERE_MODEL))));
	skybox = new BoundingBox{ -1, -1, -1, 1, 1, 1 };
	skyboxMesh = new IndexedMesh(skybox->toShape(new Vec3[8]));
	skyboxTexture = new CubeMap("../res/skybox/right.jpg", "../res/skybox/left.jpg", "../res/skybox/top.jpg", "../res/skybox/bottom.jpg", "../res/skybox/front.jpg", "../res/skybox/back.jpg");


	// Camera init
	camera.setPosition(Vec3(1, 1, -2));
	camera.setRotation(Vec3(0, 3.1415, 0.0));


	// Framebuffer init
	quad = new Quad();
	modelFrameBuffer = new FrameBuffer(width, height);
	screenFrameBuffer = new FrameBuffer(width, height);


	// Font init
	font = new Font(fontShader, "../res/fonts/droid.ttf");


	// GUI init
	GUI::init(&guiShader, font);
	panel = new Panel(0, 0, 0.2, 0.2);
	label1 = new Label("First", 0.6, 0, GUI::defaultFontSize, Vec4(1, 0, 0, 1));
	label2 = new Label("Second", 0, 0.2, GUI::defaultFontSize, Vec4(0, 1, 0, 1));
	label3 = new Label("Third", 0, 0.4, GUI::defaultFontSize, Vec4(0, 0, 1, 1));
	//panel->add(label1);
	//panel->add(label2);
	//panel->add(label3);


	// Origin init
	double originVertices[3] = { 0, 0, 5 };
	originMesh = new ArrayMesh(originVertices, 1, 3, RenderMode::POINTS);


	// Vector init
	vectorMesh = new VectorMesh(new double[128 * 7], 128);
	

	// Handler init
	handler = new StandardInputHandler(window, *this);

	eventHandler.setPhysicalRayIntersectCallback([] (Screen& screen, Physical* physical, Vec3 point) {
		screen.intersectedPhysical = physical;
		screen.intersectedPoint = point;
	});

	eventHandler.setPhysicalClickCallback([] (Screen& screen, Physical* physical, Vec3 point) {
		screen.selectedPhysical = physical;
		screen.selectedPoint = point;
	});

	eventHandler.setWindowResizeCallback([] (Screen& screen, unsigned int width, unsigned int height) {
		screen.modelFrameBuffer->texture->resize(width, height);
		screen.screenFrameBuffer->texture->resize(width, height);
		screen.modelFrameBuffer->renderBuffer->resize(width, height);
		screen.screenFrameBuffer->renderBuffer->resize(width, height);
		screen.screenSize = Vec2(width, height);
		screen.aspect = ((double) width) / ((double) height);
	});
}

void Screen::update() {

	// IO events
	static double speed = 0.02;
	if (handler->anyKey) {
		bool leftDragging = handler->leftDragging;
		if (handler->getKey(GLFW_KEY_1))  speed = 0.02;
		if (handler->getKey(GLFW_KEY_2))  speed = 0.1;		
		if (handler->getKey(GLFW_KEY_W))  camera.move(*this, 0, 0, -speed, leftDragging);
		if (handler->getKey(GLFW_KEY_S))  camera.move(*this, 0, 0, speed, leftDragging);	
		if (handler->getKey(GLFW_KEY_D))  camera.move(*this, speed, 0, 0, leftDragging);
		if (handler->getKey(GLFW_KEY_A))  camera.move(*this, -speed, 0, 0, leftDragging);
		if (handler->getKey(GLFW_KEY_SPACE)) 
			if (camera.flying) camera.move(*this, 0, speed, 0, leftDragging);
			else camera.jump(*this, leftDragging);	
		if (handler->getKey(GLFW_KEY_LEFT_SHIFT)) 
			if (camera.flying) camera.move(*this, 0, -speed, 0, leftDragging);
		if (handler->getKey(GLFW_KEY_LEFT))  camera.rotate(*this, 0, -speed, 0, leftDragging);
		if (handler->getKey(GLFW_KEY_RIGHT)) camera.rotate(*this, 0, speed, 0, leftDragging);
		if (handler->getKey(GLFW_KEY_UP))    camera.rotate(*this, -speed, 0, 0, leftDragging);
		if (handler->getKey(GLFW_KEY_DOWN))  camera.rotate(*this, speed, 0, 0, leftDragging);
		if (handler->getKey(GLFW_KEY_ESCAPE)) exit(0);
	}


	// Update camera
	camera.update();
	

	// Update lights
	/*static long long t = 0;
	float d = 0.5 + 0.5 * sin(t++ * 0.005);
	sunDirection = Vec3f(0, cos(t * 0.005) , sin(t * 0.005));
	lights[0].color = Vec3f(d, 0.3, 1-d);
	lights[1].color = Vec3f(1-d, 0.3, 1 - d);
	lights[2].color = Vec3f(0.2, 0.3*d, 1 - d);
	lights[3].color = Vec3f(1-d, 1-d, d);*/


	// Update render uniforms
	projectionMatrix = perspective(1.0, aspect, 0.01, 1000000.0);
	orthoMatrix = ortho(0, aspect, 0, 1, -1000, 1000);
	rotatedViewMatrix = camera.cframe.asMat4f().getRotation();
	viewMatrix = rotatedViewMatrix.translate(-camera.cframe.position.x, -camera.cframe.position.y, -camera.cframe.position.z);
	viewPosition = Vec3f(camera.cframe.position.x, camera.cframe.position.y, camera.cframe.position.z);


	// Update picker
	updateIntersectedPhysical(*this, world->physicals, handler->curPos, screenSize, viewMatrix, projectionMatrix);
}

AddableBuffer<double> visibleVecs(700);

void updateVecMesh(AppDebug::ColoredVec* data, size_t size) {
	visibleVecs.clear();

	for(size_t i = 0; i < size; i++) {
		const AppDebug::ColoredVec& v = data[i];
		if(debug_enabled[v.type]) {
			visibleVecs.add(v.origin.x);
			visibleVecs.add(v.origin.y);
			visibleVecs.add(v.origin.z);
			visibleVecs.add(v.vec.x);
			visibleVecs.add(v.vec.y);
			visibleVecs.add(v.vec.z);
			visibleVecs.add(vecColors[v.type]);
		}
	}

	vectorMesh->update(visibleVecs.data, visibleVecs.index / 7);
}

void Screen::renderSkybox() {
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	skyboxShader.update(sunDirection);
	skyboxShader.update(viewMatrix, projectionMatrix);
	skyboxTexture->bind();
	sphere->render();
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
}

void Screen::renderPhysicals() {
	// Bind basic uniforms
	basicShader.updateLight(lights, lightCount);
	basicShader.update(viewMatrix, projectionMatrix, viewPosition);
	basicShader.updateMaterial(material);

	SharedLockGuard lg(world->lock);
	// Render world objects
	for (Physical& physical : world->physicals) {
		int meshId = physical.part.drawMeshId;

		// Picker code
		if (&physical == selectedPhysical)
			material.ambient = Vec3f(0.5, 0.6, 0.3);
		else if (&physical == intersectedPhysical)
			material.ambient = Vec3f(0.5, 0.5, 0.5);
		else
			material.ambient = Vec3f(0.3, 0.4, 0.2);

		
		basicShader.updateMaterial(material);

		// Render each physical
		Mat4f transformation = physical.part.cframe.asMat4f();
		basicShader.updateModel(transformation);

		meshes[meshId]->render();
	}
}

template<typename T>
void Screen::renderDebugField(const char* varName, T value, const char* unit) {
	std::stringstream ss;
	ss.precision(4);
	ss << varName << ": " << value << unit;
	font->render(ss.str().c_str(), Vec2(-screenSize.x / screenSize.y * 0.99, (1 - fieldIndex * 0.05) * 0.95), Vec3f(1,1,1), 0.001);
	fieldIndex++;
}

std::string toString(std::chrono::nanoseconds t) {
	std::stringstream ss;
	ss.precision(4);
	ss << t.count() * 0.000001f;
	ss << "ms";
	return ss.str();
}

std::string toString(size_t v) {
	std::stringstream ss;
	ss << v;
	return ss.str();
}

template<size_t N, typename EnumType>
PieChart toPieChart(BreakdownAverageProfiler<N, EnumType>& profiler, const char* title, Vec2f piePosition, float pieSize) {
	auto results = profiler.history.avg();
	std::chrono::nanoseconds avgTotalTime = results.sum();

	PieChart chart(title, toString(avgTotalTime), piePosition, pieSize);
	for(size_t i = 0; i < profiler.size(); i++) {
		float weight = static_cast<float>(results[i].count());
		PiePart p = PiePart(weight, toString(results[i]), pieColors[i], profiler.labels[i]);
		chart.add(p);
	}

	return chart;
}

template<size_t N, typename Unit, typename EnumType>
PieChart toPieChart(HistoricTally<N, Unit, EnumType>& tally, const char* title, Vec2f piePosition, float pieSize) {
	auto sum = 0;
	for(auto entry : tally.history) {
		sum += entry.sum();
	}
	auto results = tally.history.avg();
	Unit avgTotal = (tally.history.size() != 0)? (sum / tally.history.size()) : 0;
	//Unit avgTotal = results.sum();

	PieChart chart(title, toString(avgTotal), piePosition, pieSize);
	for(size_t i = 0; i < tally.size(); i++) {
		float result = results[i] * 1.0f;
		PiePart p = PiePart(result, toString(results[i]), pieColors[i], tally.labels[i]);
		chart.add(p);
	}

	return chart;
}

size_t getTheoreticalNumberOfIntersections(size_t objCount) {
	return (objCount-1)*objCount / 2;
}

void Screen::refresh() {
	fieldIndex = 0;

	// Render skybox
	graphicsMeasure.mark(GraphicsProcess::SKYBOX);
	modelFrameBuffer->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderSkybox();

	glEnable(GL_DEPTH_TEST);


	// Initialize vector log buffer
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	AddableBuffer<AppDebug::ColoredVec> vecLog = AppDebug::getVecBuffer();


	// Render physicals
	graphicsMeasure.mark(GraphicsProcess::PHYSICALS);
	renderPhysicals();


	// Postprocess to screenFrameBuffer
	screenFrameBuffer->bind();
	glDisable(GL_DEPTH_TEST);
	postProcessShader.bind();
	modelFrameBuffer->texture->bind();
	quad->render();


	// Render vectors with old depth buffer
	glEnable(GL_DEPTH_TEST);
	screenFrameBuffer->attach(modelFrameBuffer->renderBuffer);
	

	// Update vector mesh
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	updateVecMesh(vecLog.data, vecLog.index);


	// Render lights
	graphicsMeasure.mark(GraphicsProcess::LIGHTING);
	for (Light light : lights) {
		Mat4f transformation = Mat4f().translate(light.position).scale(0.1);
		basicShader.updateMaterial(Material(light.color, Vec3f(), Vec3f(), 10));
		basicShader.updateModel(transformation);
		skyboxMesh->render();
	}
	

	// Render vector mesh
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	vectorShader.update(viewMatrix, projectionMatrix, viewPosition);
	vectorMesh->render();


	// Render origin mesh
	graphicsMeasure.mark(GraphicsProcess::ORIGIN);
	originShader.update(viewMatrix, rotatedViewMatrix, projectionMatrix, orthoMatrix, viewPosition);
	originMesh->render();


	// Render GUI
	glDisable(GL_DEPTH_TEST);
	graphicsMeasure.mark(GraphicsProcess::OTHER);
	fontShader.update(orthoMatrix);
	GUI::update(orthoMatrix);
	label1->text = "Objects: " + std::to_string(world->physicals.size());
	panel->position = Vec2(0.1, 0.7);
	panel->render();

	// Pie rendering
	graphicsMeasure.mark(GraphicsProcess::PROFILER);
	size_t objCount = world->physicals.physicalCount;
	renderDebugField("Objects", objCount, "");
	renderDebugField("Intersections", getTheoreticalNumberOfIntersections(objCount), "");

	renderDebugField("TPS", physicsMeasure.getAvgTPS(), "");
	renderDebugField("FPS", graphicsMeasure.getAvgTPS(), "");

	


	if(renderPies) {
		float leftSide = screenSize.x / screenSize.y;
		PieChart graphicsPie = toPieChart(graphicsMeasure, "Graphics", Vec2f(-leftSide + 1.5f, -0.7f), 0.2f);
		PieChart physicsPie = toPieChart(physicsMeasure, "Physics", Vec2f(-leftSide + 0.3f, -0.7f), 0.2f);
		PieChart intersectionPie = toPieChart(intersectionStatistics, "Intersection Statistics", Vec2f(-leftSide + 2.7f, -0.7f), 0.2f);
		physicsPie.renderText(*this, font);
		graphicsPie.renderText(*this, font);
		intersectionPie.renderText(*this, font);
		startPieRendering(*this);
		physicsPie.renderPie(*this);
		graphicsPie.renderPie(*this);
		intersectionPie.renderPie(*this);
		endPieRendering(*this);
	}

	// Render screenFrameBuffer texture to the screen
	graphicsMeasure.mark(GraphicsProcess::FINALIZE);
	screenFrameBuffer->unbind();
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);


	// Render postprocessed image to screen
	quadShader.bind();
	screenFrameBuffer->texture->bind();
	quad->render();


	// Render stuff
	glfwSwapBuffers(this->window);
	glfwPollEvents();
}

void Screen::close() {
	basicShader.close();
	basicNormalShader.close();
	vectorShader.close();
	fontShader.close();
	originShader.close();
	skyboxShader.close();
	quadShader.close();
	postProcessShader.close();

	terminateGL();
}

bool Screen::shouldClose() {
	return glfwWindowShouldClose(window) != 0;
}

int Screen::addMeshShape(Shape s) {
	int size = meshes.size();
	meshes.push_back(new IndexedMesh(s));
	return size;
}

void Screen::toggleDebugVecType(Debug::VecType t) {
	debug_enabled[t] = !debug_enabled[t];
}
