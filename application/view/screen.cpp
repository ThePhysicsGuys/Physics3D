#include "screen.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "renderUtils.h"
#include "texture.h"
#include "material.h"
#include "light.h"
#include "shader.h"
#include "shaderProgram.h"
#include "font.h"

#include "mesh/indexedMesh.h"
#include "mesh/arrayMesh.h"
#include "mesh/vectorMesh.h"
#include "mesh/primitive.h"
#include "mesh/pointMesh.h"
#include "picker/picker.h"
#include "debug/visualDebug.h"
#include "buffers/frameBuffer.h"

#include "gui/panel.h"
#include "gui/frame.h"
#include "gui/label.h"
#include "gui/image.h"
#include "gui/slider.h"
#include "gui/checkBox.h"
#include "gui/colorPicker.h"
#include "gui/directionEditor.h"
#include "gui/gui.h"
#include "gui/frames.h"

#include "../debug.h"
#include "../options/keyboardOptions.h"
#include "../input/standardInputHandler.h"
#include "../resourceManager.h"
#include "../objectLibrary.h"
#include "../visualShape.h"
#include "../io/import.h"
#include "../util/log.h"

#include "../engine/debug.h"
#include "../engine/profiling.h"
#include "../engine/math/vec2.h"
#include "../engine/math/mat4.h"
#include "../engine/math/mathUtil.h"
#include "../engine/geometry/shape.h"
#include "../engine/geometry/shape.h"
#include "../engine/sharedLockGuard.h"
#include "../engine/physicsProfiler.h"
#include "../engine/geometry/boundingBox.h"
#include "../engine/datastructures/buffers.h"

#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <math.h>
#include <map>

#include "../worlds.h"

#include "../engine/math/tempUnsafeCasts.h"

bool initGLFW() {

	// Set window hints
	//Renderer::setGLFWMultisampleSamples(4);

	// Initialize GLFW
	if (!Renderer::initGLFW()) {
		Log::error("GLFW failed to initialize");
		return false;
	}

	//Renderer::enableMultisampling();

	Log::info("Initialized GLFW");

	return true;
}

bool initGLEW() {
	// Init GLEW after creating a valid rendering context
	if (!Renderer::initGLEW()) {
		terminateGLFW();

		Log::error("GLEW Failed to initialize!");

		return false;
	}

	Log::info("Initialized GLEW");
	return true;
}

void terminateGLFW() {
	Log::info("Closing GLFW");
	Renderer::terminateGLFW();
	Log::info("Closed GLFW");
}

Screen::Screen() {};

Screen::Screen(int width, int height, MagnetWorld* world) {
	this->world = world;

	// Create a windowed mode window and its OpenGL context 
	Renderer::createGLFWContext(width, height, "Physics3D");
	
	if (!Renderer::validGLFWContext()) {
		Log::fatal("Invalid rendering context");
		terminateGLFW();
		exit(-1);
	}

	// Make the window's context current 
	Renderer::makeGLFWContextCurrent();

	Log::info("OpenGL vendor: (%s)", Renderer::getVendor());
	Log::info("OpenGL renderer: (%s)", Renderer::getRenderer());
	Log::info("OpenGL version: (%s)", Renderer::getVersion());
	Log::info("OpenGL shader version: (%s)", Renderer::getShaderVersion());
}


// Generic Shapes
IndexedMesh* sphere = nullptr;
IndexedMesh* cube = nullptr;
IndexedMesh* plane = nullptr;


// Font
Font* font = nullptr;


// Handler
StandardInputHandler* handler = nullptr;


// Textures
Texture* floorTexture = nullptr;
Texture* floorNormal = nullptr;


// Skybox 
BoundingBox* skybox = nullptr;
CubeMap* skyboxTexture = nullptr;
IndexedMesh* skyboxMesh = nullptr;


// Render uniforms
Mat4f orthoMatrix;


// Debug
BarChartClassInformation iterChartClasses[]{ {"GJK Collide", Vec3f(0.2f,0.2f,1)},{"GJK No Collide", Vec3f(1.0f, 0.5f, 0.0f)},{"EPA", Vec3f(1.0f, 1.0f, 0.0f)} };
BarChart iterationChart("Iteration Statistics", "", GJKCollidesIterationStatistics.labels, iterChartClasses, Vec2f(-1 + 0.1f, -0.3), Vec2f(0.8, 0.6), 3, 17);


// Light uniforms
Vec3f sunDirection;
Vec3f sunColor;
Attenuation attenuation = { 0, 0, 0.5 };
const int lightCount = 5;
Light lights[lightCount] = {
	Light(Vec3f(10, 5, -10), Vec3f(1, 0.84, 0.69), 6, attenuation),
	Light(Vec3f(10, 5, 10), Vec3f(1, 0.84, 0.69), 6, attenuation),
	Light(Vec3f(-10, 5, -10), Vec3f(1, 0.84, 0.69), 6, attenuation),
	Light(Vec3f(-10, 5, 10), Vec3f(1, 0.84, 0.69), 6, attenuation),
	Light(Vec3f(0, 10, 0), Vec3f(1, 0.90, 0.75), 10, attenuation)
};


// Shadow
DepthFrameBuffer* depthBuffer = nullptr;


// Render meshes
VectorMesh* vectorMesh = nullptr;
PointMesh* pointMesh = nullptr;
ArrayMesh* originMesh = nullptr;


// Frames
PropertiesFrame* propertiesFrame = nullptr;
DebugFrame* debugFrame = nullptr;
EnvironmentFrame* environmentFrame = nullptr;


// Test
IndexedMesh* mesh = nullptr;
Texture* texture = nullptr;

void Screen::init() {
	// Log init
	Log::setLogLevel(Log::Level::INFO);


	// Properties init
	properties = PropertiesParser::read("../res/.properties");

	// load options from properties
	KeyboardOptions::load(properties);


	// Render mode init
	Renderer::enableCulling();
	Renderer::enableDepthTest();


	// InputHandler init
	handler = new StandardInputHandler(Renderer::getGLFWContext(), *this);


	// Screen size init
	dimension = Renderer::getGLFWWindowSize();


	// Framebuffer init
	quad = new Quad();
	modelFrameBuffer = new HDRFrameBuffer(dimension.x, dimension.y);
	screenFrameBuffer = new FrameBuffer(dimension.x, dimension.y);
	maskFrameBuffer = new FrameBuffer(dimension.x, dimension.y);
	blurFrameBuffer = new FrameBuffer(dimension.x, dimension.y);
	depthBuffer = new DepthFrameBuffer(1024, 1024);


	// Eventhandler init
	eventHandler.setWindowResizeCallback([](Screen& screen, Vec2i dimension) {
		screen.screenFrameBuffer->resize(dimension);
		screen.maskFrameBuffer->resize(dimension);
		screen.modelFrameBuffer->resize(dimension);
		screen.blurFrameBuffer->resize(dimension);
		GUI::guiFrameBuffer->resize(dimension);

		screen.camera.update(((float)dimension.x) / ((float)dimension.y));
		screen.dimension = dimension;
	});


	// Shader init
	Shaders::init();


	// Light init
	Shaders::basicShader.createLightArray(lightCount);


	// Texture init
	/*floorTexture = load("../res/textures/floor/floor.jpg");
	//floorNormal = load("../res/textures/metal/metal_normal.jpg");
	floorNormal = load("../res/textures/floor/floor_normal.jpg");
	if(floorTexture != nullptr)
		defaultMaterial.setTexture(floorTexture);
	if(floorNormal != nullptr)
		defaultMaterial.setNormalMap(floorNormal);*/


	// Skybox init
	VisualShape sphereShape(OBJImport::load((std::istream&) std::istringstream(getResourceAsString(SPHERE_MODEL))));
	sphere = new IndexedMesh(sphereShape);
	cube = new IndexedMesh(VisualShape(createCube(1.0)));
	skybox = new BoundingBox{ -1, -1, -1, 1, 1, 1 };
	skyboxMesh = new IndexedMesh(VisualShape(skybox->toShape()));
	skyboxTexture = new CubeMap("../res/skybox/right.jpg", "../res/skybox/left.jpg", "../res/skybox/top.jpg", "../res/skybox/bottom.jpg", "../res/skybox/front.jpg", "../res/skybox/back.jpg");


	// Camera init
	camera.setPosition(Position(1.0, 1.0, -2.0));
	camera.setRotation(Vec3(0, 3.1415, 0.0));
	camera.update(1.0, camera.aspect, 0.01, 10000.0);


	// Font init
	font = new Font("../res/fonts/droid.ttf");


	// GUI init
	GUI::init(this, font);


	// Resize
	handler->framebufferResize(dimension);


	// Frames init
	propertiesFrame = new PropertiesFrame(0.75, 0.75);
	environmentFrame = new EnvironmentFrame(0.8, 0.8);
	debugFrame = new DebugFrame(0.7, 0.7);
	

	// Picker init
	Picker::init();


	// Origin init
	float originVertices[3] = { 0, 0, 5 };
	originMesh = new ArrayMesh(originVertices, 1, 3, RenderMode::POINTS);
	

	// Vector init
	vectorMesh = new VectorMesh(new float[128 * 9], 128);
	
	
	// Point init
	float* buf = new float[5 * 10];
	for (int i = 0; i < 5; i++) {
		buf[i * 10 + 0] = 0.5f + i;
		buf[i * 10 + 1] = 1.0f; // position
		buf[i * 10 + 2] = 0.5f;

		buf[i * 10 + 3] = 0.02f * i; // size

		buf[i * 10 + 4] = 1.0f;
		buf[i * 10 + 5] = 1.0f; // color 1 orange
		buf[i * 10 + 6] = 0.0f;

		buf[i * 10 + 7] = 0.0f;
		buf[i * 10 + 8] = 0.0f; // color 2 white
		buf[i * 10 + 9] = 0.0f;

	}

	pointMesh = new PointMesh(buf, 5);


	// Test
	VisualShape planeShape(OBJImport::load("../res/models/plane.obj"));
	plane = new IndexedMesh(planeShape);
	texture = load("../res/textures/test/disp.jpg");
	BoundingBox box = BoundingBox{ -1,-1,-1,1,1,1 };
	VisualShape shape = VisualShape(box.toShape());
	Vec3f* buffer = new Vec3f[shape.vertexCount];
	shape.computeNormals(buffer);
	shape.normals = SharedArrayPtr<const Vec3f>(buffer);
	mesh = new IndexedMesh(shape);
}

void Screen::update() {

	// IO events
	if (handler->anyKey) {
		bool leftDragging = handler->leftDragging;
		if (handler->getKey(KeyboardOptions::Move::forward))  camera.move(*this, 0, 0, -1, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::backward))  camera.move(*this, 0, 0, 1, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::right))  camera.move(*this, 1, 0, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::left))  camera.move(*this, -1, 0, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::ascend))
			if (camera.flying) camera.move(*this, 0, 1, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Move::descend))
			if (camera.flying) camera.move(*this, 0, -1, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Rotate::left))  camera.rotate(*this, 0, -1, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Rotate::right)) camera.rotate(*this, 0, 1, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Rotate::up))    camera.rotate(*this, -1, 0, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Rotate::down))  camera.rotate(*this, 1, 0, 0, leftDragging);
		if (handler->getKey(KeyboardOptions::Application::close)) Renderer::closeGLFWWindow();
		if (handler->getKey(KeyboardOptions::Debug::frame)) { debugFrame->visible = true; debugFrame->position = Vec2(0.8); GUI::select(debugFrame); }
	}


	// Update camera
	camera.update();


	/*// Update lights
	static long long t = 0;
	float d = 0.5 + 0.5 * sin(t++ * 0.005);
	sunDirection = Vec3f(0, cos(t * 0.005) , sin(t * 0.005));
	lights[0].color = Vec3f(d, 0.3, 1-d);
	lights[1].color = Vec3f(1-d, 0.3, 1 - d);
	lights[2].color = Vec3f(0.2, 0.3*d, 1 - d);
	lights[3].color = Vec3f(1-d, 1-d, d);*/


	// Update render uniforms
	orthoMatrix = ortho(-camera.aspect, camera.aspect, -1.0f, 1.0f, -1000.0f, 1000.0f);
	

	// Update picker
	Picker::update(*this, handler->cursorPosition);


	// Update gui
	// Update GUI intersection
	GUI::intersect(GUI::map(handler->cursorPosition));
	
	// Update frames
	propertiesFrame->update();
	debugFrame->update();
	environmentFrame->update();

}

void Screen::renderSkybox() {
	Renderer::disableDepthMask();
	Renderer::disableCulling();
	Renderer::enableBlending();
	Renderer::standardBlendFunction();
	Shaders::skyboxShader.updateLightDirection(sunDirection);
	Shaders::skyboxShader.updateProjection(camera.viewMatrix, camera.projectionMatrix);
	skyboxTexture->bind();
	sphere->render();
	Renderer::enableDepthMask();
	Renderer::enableCulling();
}

void Screen::renderPhysicals() {
	std::map<double, ExtendedPart*> transparentMeshes;

	// Bind basic uniforms
	Shaders::basicShader.updateProjection(camera.viewMatrix, camera.projectionMatrix, camera.cframe.position);
	Shaders::basicShader.updateLight(lights, lightCount);
	Shaders::maskShader.updateProjection(camera.viewMatrix, camera.projectionMatrix);
	
	// Render world objects
	for (ExtendedPart& part : *world) {
		int meshId = part.drawMeshId;

		Material material = part.material;

		// Picker code
		if (&part == intersectedPart)
			material.ambient = part.material.ambient + Vec4f(-0.1, -0.1, -0.1, 0);
		else
			material.ambient = part.material.ambient;
		
		if (material.ambient.w < 1) {
			transparentMeshes[Vec3(camera.cframe.position - TEMP_CAST_VEC_TO_POSITION(part.cframe.position)).lengthSquared()] = &part;
			continue;
		}

		Shaders::basicShader.updateMaterial(material);

		// Render each physical
		Shaders::basicShader.updatePart(part);

		if(meshId == -1) continue;

		/*if (&part == selectedPart) {
			maskFrameBuffer->bind();
			Renderer::clearDepth();
			Renderer::clearColor();
			Shaders::maskShader.updateModel(CFrameToMat4(part.cframe));
			meshes[meshId]->render();
			Shaders::edgeShader.updateTexture(maskFrameBuffer->texture);
			quad->render();
			image->texture = maskFrameBuffer->texture;
			modelFrameBuffer->bind();
		} else {*/
			meshes[meshId]->render(part.renderMode);
		//}
	}

	for (auto iterator = transparentMeshes.rbegin(); iterator != transparentMeshes.rend(); ++iterator) {
		ExtendedPart* part = (*iterator).second;
		
		Material material = part->material;

		if (part == selectedPart)
			material.ambient = part->material.ambient + Vec4f(0.1, 0.1, 0.1, -0.2);
		else if (part == intersectedPart)
			material.ambient = part->material.ambient + Vec4f(-0.1, -0.1, -0.1, 0);
		else
			material.ambient = part->material.ambient;
		
		Shaders::basicShader.updateMaterial(material);
	   
		// Render each physical
		Shaders::basicShader.updatePart(*part);

		if (part->drawMeshId == -1) continue;

		meshes[part->drawMeshId]->render(part->renderMode);
	}
}

void renderSphere(double radius, Vec3 position, Vec4f color) {
	Shaders::basicShader.updateMaterial(Material(color));

	Shaders::basicShader.updateModel(CFrameToMat4(CFrame(position, DiagonalMat3(1,1,1)*radius)));

	sphere->render();
}

void renderBox(const CFrame& cframe, double width, double height, double depth, Vec4f color) {
	Shaders::basicShader.updateMaterial(Material(color));

	Shaders::basicShader.updateModel(CFrameToMat4(CFrame(cframe.getPosition(), cframe.getRotation() * DiagonalMat3(width, height, depth))));

	cube->render();
}

void renderBounds(const Bounds& bounds, const Vec4f& color) {
	Vec3Fix diagonal = bounds.getDiagonal();
	Position p = bounds.getCenter();
	renderBox(CFrame(Vec3(p.x, p.y, p.z)), diagonal.x, diagonal.y, diagonal.z, color);
}

Vec4f colors[] {
	GUI::COLOR::BLUE,
	GUI::COLOR::GREEN,
	GUI::COLOR::YELLOW,
	GUI::COLOR::ORANGE,
	GUI::COLOR::RED,
	GUI::COLOR::PURPLE
};

void recursiveRenderColTree(const TreeNode& node, int depth) {
	if (node.isLeafNode()) {
		//renderBounds(node.bounds, GUI::COLOR::AQUA);

	} else {
		for (const TreeNode& node : node) {
			recursiveRenderColTree(node, depth + 1);
		}
	}

	Vec4f color = colors[depth % 6];
	color.w = 0.3;

	renderBounds(node.bounds.expanded((5 - depth) * 0.002), color);
}

bool recursiveColTreeForOneObject(const TreeNode & node, const Physical * obj, const Bounds & bounds) {
	if (node.isLeafNode()) {
		if (node.object == obj)
			return true;
		/*for (const BoundedPhysical& p : *node.physicals) {
			if (p.object == obj) {
				return true;
			}
		}*/
	} else {
		//if (!intersects(node.bounds, bounds)) return false;
		for (const TreeNode& subNode : node) {
			if (recursiveColTreeForOneObject(subNode, obj, bounds)) {
				Vec4f orange = GUI::COLOR::GREEN;
				orange.w = 0.3;

				renderBounds(node.bounds, orange);
				return true;
			}
		}
	}
	return false;
}

void Screen::renderDebug() {
	// Initialize vector log buffer
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	AddableBuffer<AppDebug::ColoredVector>& vecLog = AppDebug::getVectorBuffer();
	AddableBuffer<AppDebug::ColoredPoint>& pointLog = AppDebug::getPointBuffer();

	for (const Physical& p : world->iterPhysicals()) {
		pointLog.add(AppDebug::ColoredPoint(p.getCenterOfMass(), Debug::CENTER_OF_MASS));
	}

	if (selectedPart != nullptr) {
		CFramef selectedCFrame(selectedPart->cframe);
		for (const Vec3f& corner : selectedPart->hitbox.iterVertices()) {
			vecLog.add(AppDebug::ColoredVector(Vec3(selectedCFrame.localToGlobal(corner)), selectedPart->parent->getVelocityOfPoint(Vec3(selectedCFrame.localToRelative(corner))), Debug::VELOCITY));
		}

		if (colissionSpheresMode == SphereColissionRenderMode::SELECTED) {
			Physical& selectedPhys = *selectedPart->parent;

			for (Part& part : selectedPhys) {
				Vec4f yellow = GUI::COLOR::YELLOW;
				yellow.w = 0.5;
				BoundingBox localBounds = selectedPart->localBounds;
				renderBox(selectedPart->cframe.localToGlobal(CFrame(localBounds.getCenter())), localBounds.getWidth(), localBounds.getHeight(), localBounds.getDepth(), yellow);

				Vec4f green = GUI::COLOR::GREEN;
				green.w = 0.5;
				renderSphere(part.maxRadius * 2, part.cframe.getPosition(), green);
			}

			Vec4f red = GUI::COLOR::RED;
			red.w = 0.5;
			BoundingBox localBounds = selectedPhys.localBounds;
			renderBox(selectedPhys.getCFrame().localToGlobal(CFrame(localBounds.getCenter())), localBounds.getWidth(), localBounds.getHeight(), localBounds.getDepth(), red);

			Vec4f blue = GUI::COLOR::BLUE;
			blue.w = 0.5;
			renderSphere(selectedPhys.circumscribingSphere.radius * 2, selectedPhys.circumscribingSphere.origin, blue);
		}
	}

	if (colissionSpheresMode == SphereColissionRenderMode::ALL) {
		for (Physical& phys : world->iterPhysicals()) {
			for (Part& part : phys) {
				Vec4f yellow = GUI::COLOR::YELLOW;
				yellow.w = 0.5;
				BoundingBox localBounds = part.localBounds;
				renderBox(part.cframe.localToGlobal(CFrame(localBounds.getCenter())), localBounds.getWidth(), localBounds.getHeight(), localBounds.getDepth(), yellow);

				Vec4f green = GUI::COLOR::GREEN;
				green.w = 0.5;
				renderSphere(part.maxRadius * 2, part.cframe.getPosition(), green);
			}
		}

		for (Physical& phys : world->iterPhysicals()) {
			Vec4f red = GUI::COLOR::RED;
			red.w = 0.5;
			BoundingBox localBounds = phys.localBounds;
			renderBox(phys.getCFrame().localToGlobal(CFrame(localBounds.getCenter())), localBounds.getWidth(), localBounds.getHeight(), localBounds.getDepth(), red);

			Vec4f blue = GUI::COLOR::BLUE;
			blue.w = 0.5;
			renderSphere(phys.circumscribingSphere.radius * 2, phys.circumscribingSphere.origin, blue);
		}
	}
	switch (renderColTree) {
		case ColTreeRenderMode::ALL:
			recursiveRenderColTree(world->objectTree.rootNode, 0);
			break;
		case ColTreeRenderMode::SELECTED:
			if (selectedPart != nullptr)
				recursiveColTreeForOneObject(world->objectTree.rootNode, selectedPart->parent, selectedPart->parent->getStrictBounds());
			break;
	}

	// Update debug meshes
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	updateVecMesh(vectorMesh, vecLog.data, vecLog.size);
	updatePointMesh(pointMesh, pointLog.data, pointLog.size);
}

void Screen::renderPies() {
	graphicsMeasure.mark(GraphicsProcess::PROFILER);
	size_t objCount = world->getPartCount();
	renderDebugField(dimension, font, "Screen", str(dimension) + ", [" + std::to_string(camera.aspect) + ":1]", "");
	renderDebugField(dimension, font, "Position", str(camera.cframe.position), "");
	renderDebugField(dimension, font, "Objects", objCount, "");
	renderDebugField(dimension, font, "Intersections", getTheoreticalNumberOfIntersections(objCount), "");
	renderDebugField(dimension, font, "AVG Collide GJK Iterations", gjkCollideIterStats.avg(), "");
	renderDebugField(dimension, font, "AVG No Collide GJK Iterations", gjkNoCollideIterStats.avg(), "");
	renderDebugField(dimension, font, "TPS", physicsMeasure.getAvgTPS(), "");
	renderDebugField(dimension, font, "FPS", graphicsMeasure.getAvgTPS(), "");
	renderDebugField(dimension, font, "World Kinetic Energy", world->getTotalKineticEnergy(), "");
	renderDebugField(dimension, font, "World Potential Energy", world->getTotalPotentialEnergy(), "");
	renderDebugField(dimension, font, "World Energy", world->getTotalEnergy(), "");
	renderDebugField(dimension, font, "World Age", world->age, " ticks");

	if (renderPiesEnabled) {
		float leftSide = float(dimension.x) / float(dimension.y);
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

		ParallelArray<long long, 17> gjkColIter = GJKCollidesIterationStatistics.history.avg();
		ParallelArray<long long, 17> gjkNoColIter = GJKNoCollidesIterationStatistics.history.avg();
		ParallelArray<long long, 17> epaIter = EPAIterationStatistics.history.avg();

		for (size_t i = 0; i < GJKCollidesIterationStatistics.size(); i++) {
			iterationChart.data[0][i] = WeightValue{ (float)gjkColIter[i], std::to_string(gjkColIter[i]) };
			iterationChart.data[1][i] = WeightValue{ (float)gjkNoColIter[i], std::to_string(gjkNoColIter[i]) };
			iterationChart.data[2][i] = WeightValue{ (float)epaIter[i], std::to_string(epaIter[i]) };
		}

		iterationChart.position = Vec2f(-leftSide + 0.1f, -0.3);
		iterationChart.render();
	}
}

void Screen::render() {
	fieldIndex = 0;

	//Mat4f f = CFrameToMat4(camera.cframe);
	//camera.cframe = Mat4ToCFrame(lookAt(f, camera.cframe.position, Vec3f()));

	// Shadow setup
	/*Mat4f lightProjection = ortho(-camera.aspect, camera.aspect, -1.0f, 1.0f, 0.1f, 1000.0f);
	Mat4f ligthView = lookAt(camera.cframe.position, camera.cframe.rotation * Vec3f(0, 0, 1), Vec3f(0.0f, 1.0f, 0.0f));
	Mat4f lightMatrix = lightProjection * ligthView;
	Shaders::depthShader.updateLight(lightMatrix);
	Renderer::enableDepthTest();
	depthBuffer->bind();
	//Renderer::clearDepth();
	for (ExtendedPart& part : *world) {
		int meshId = part.drawMeshId;
		Shaders::depthShader.updateModel(CFrameToMat4(CFramef(part.cframe)));
		meshes[meshId]->render();
	}
	Renderer::viewport(Vec2i(), dimension);*/


	// Render skybox
	graphicsMeasure.mark(GraphicsProcess::SKYBOX);
	modelFrameBuffer->bind();
	Renderer::clearColor();
	Renderer::clearDepth();
	renderSkybox();
	Renderer::enableDepthTest();

	// This curly bracket is very important - Lord Von Tum 02/08/2019 00:45 AM
	{ 
		SharedLockGuard lg(world->lock);
		// Render physicals
		graphicsMeasure.mark(GraphicsProcess::PHYSICALS);
		Shaders::basicShader.updateProjection(camera.viewMatrix, camera.projectionMatrix, camera.cframe.position);
		renderPhysicals();


		// Debug 
		renderDebug();

		float rightSide = float(dimension.x) / float(dimension.y);
		if(renderPiesEnabled) renderTreeStructure(*this, world->objectTree.rootNode, Vec3f(0,1,0), Vec2f(rightSide, 0.95), 2.0);
	}

	// Test
	Renderer::disableCulling();
	Shaders::testShader.updateModel(Mat4f().translate(0, 2, 0));
	Shaders::testShader.updateView(camera.viewMatrix);
	Shaders::testShader.updateViewPosition(camera.cframe.position);
	Shaders::testShader.updateProjection(camera.projectionMatrix);
	Shaders::testShader.updateDisplacement(texture);
	plane->renderMode = RenderMode::PATCHES;
	plane->render(Renderer::WIREFRAME);
	plane->renderMode = RenderMode::QUADS;
	Renderer::enableCulling();


	// Postprocess to screenFrameBuffer
	screenFrameBuffer->bind();
	Renderer::disableDepthTest();
	Shaders::postProcessShader.updateTexture(modelFrameBuffer->texture);
	quad->render();


	// Render vectors with old depth buffer
	Renderer::enableDepthTest();
	screenFrameBuffer->attach(modelFrameBuffer->renderBuffer);
	

	// Render lights
	graphicsMeasure.mark(GraphicsProcess::LIGHTING);
	for (Light light : lights) {
		Mat4f transformation = Mat4f().translate(light.position).scale(0.1f);
		Shaders::basicShader.updateMaterial(Material(Vec4f(light.color, 1), Vec3f(), Vec3f(), 10));
		Shaders::basicShader.updateModel(transformation);
		skyboxMesh->render();
	}
	Renderer::disableDepthTest();


	// Render vector mesh
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	Shaders::vectorShader.updateProjection(camera.viewMatrix, camera.projectionMatrix, camera.cframe.position);
	vectorMesh->render();


	// Render point mesh
	graphicsMeasure.mark(GraphicsProcess::VECTORS);
	Shaders::pointShader.updateProjection(camera.viewMatrix, camera.projectionMatrix, camera.cframe.position);
	pointMesh->render();

	Renderer::enableDepthTest();


	// Render origin mesh
	graphicsMeasure.mark(GraphicsProcess::ORIGIN);
	Shaders::originShader.updateProjection(camera.viewMatrix, Mat3f(camera.cframe.rotation), camera.projectionMatrix, orthoMatrix, camera.cframe.position);
	originMesh->render();


	// Render screenFrameBuffer texture to the screen
	graphicsMeasure.mark(GraphicsProcess::FINALIZE);
	screenFrameBuffer->unbind();

	Renderer::clearColor();
	Renderer::disableDepthTest();


	// Render postprocessed image to screen
	Shaders::quadShader.updateProjection(Mat4f());
	Shaders::quadShader.updateTexture(screenFrameBuffer->texture);
	quad->render();


	// Render edit tools
	Shaders::maskShader.updateProjection(camera.viewMatrix, camera.projectionMatrix);
	Picker::render(*this);


	// Render GUI
	Renderer::disableDepthTest();
	graphicsMeasure.mark(GraphicsProcess::OTHER);
	Shaders::fontShader.updateProjection(orthoMatrix);
	GUI::render(orthoMatrix);


	// Pie rendering
	renderPies();


	// Render stuff
	Renderer::swapGLFWInterval(0);
	Renderer::swapGLFWBuffers();
	Renderer::pollGLFWEvents();
}

void Screen::close() {
	Shaders::close();

	KeyboardOptions::save(properties);

	PropertiesParser::write("../res/.properties", properties);

	terminateGLFW();
}

bool Screen::shouldClose() {
	return Renderer::isGLFWWindowClosed();
}

int Screen::addMeshShape(const VisualShape& s) {
	int size = (int) meshes.size();
	meshes.push_back(new IndexedMesh(s));
	return size;
}