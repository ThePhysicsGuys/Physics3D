// Include GLEW. Always include it before gl.h and glfw3.h, since it's a bit magic.
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <Physics3D/math/linalg/vec.h>
#include <Physics3D/math/linalg/mat.h>
#include <Physics3D/math/linalg/trigonometry.h>

#include <Physics3D/part.h>
#include <Physics3D/world.h>
#include <Physics3D/worldIteration.h>
#include <Physics3D/geometry/polyhedron.h>
#include <Physics3D/geometry/shape.h>
#include <Physics3D/geometry/shapeClass.h>
#include <Physics3D/geometry/shapeCreation.h>
#include <Physics3D/geometry/shapeLibrary.h>
#include <Physics3D/threading/physicsThread.h>
#include <Physics3D/threading/synchronizedWorld.h>
#include <Physics3D/externalforces/directionalGravity.h>

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <memory>

using namespace P3D;

GLuint LoadShaders(const char* vertexShaderSourceCode, const char* fragmentShaderSourceCode) {
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling vertex shader\n");
	glShaderSource(VertexShaderID, 1, &vertexShaderSourceCode , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling fragment shader\n");
	glShaderSource(FragmentShaderID, 1, &fragmentShaderSourceCode , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}


GLFWwindow* initWindow() {
	// Initialise GLFW
	if(!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return nullptr;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 


	// Open a window and create its OpenGL context
	GLFWwindow* window;
	window = glfwCreateWindow( 1024, 768, "Physics3D Demo", NULL, NULL);
	if(window == NULL) {
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window); // Initialize GLEW

	if(glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return nullptr;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	return window;
}


struct Color {
	union {
		float data[3];
		struct {
			float r;
			float g;
			float b;
		};
		Vec3f asVec3f;
	};
};
namespace Colors {
	static constexpr Color BLUE  {0.0f, 0.0f, 1.0f};
	static constexpr Color RED   {1.0f, 0.0f, 0.0f};
	static constexpr Color GREEN {0.0f, 1.0f, 0.0f};
	static constexpr Color WHITE {1.0f, 1.0f, 1.0f};
	static constexpr Color BLACK {0.0f, 0.0f, 0.0f};
	static constexpr Color GRAY  {0.5f, 0.5f, 0.5f};
}

const char* const basicVertexShader = R"(
#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;

uniform mat4 modelMatrix;
uniform mat4 pvMatrix;
uniform vec3 lightDirection;
uniform vec3 partColor;

out vec3 fragmentColor;
  
void main(){
  // Output position of the vertex, in clip space : pvMatrix * modelMatrix * position
  gl_Position = pvMatrix * modelMatrix * vec4(vertexPosition_modelspace,1);
  vec3 vertexNormal_worldspace = mat3(modelMatrix) * vertexNormal_modelspace;
  float brightness = (dot(-vertexNormal_worldspace, lightDirection) + 1) / 2;
  //fragmentColor = vec3(brightness, brightness, brightness);
  fragmentColor = brightness * partColor;
}
)";

const char* basicFragmentShader = R"(
#version 330 core
out vec3 color;
in vec3 fragmentColor;
void main(){
  color = fragmentColor;
}
)";
class RenderShader {
	GLuint programID;

	GLuint modelMatrixID;
	GLuint pvMatrixID;
	GLuint lightDirectionID;
	GLuint partColorID;

public:
	RenderShader() {
		GLuint programID = LoadShaders(basicVertexShader, basicFragmentShader);
		glUseProgram(programID);
		
		modelMatrixID = glGetUniformLocation(programID, "modelMatrix");
		pvMatrixID = glGetUniformLocation(programID, "pvMatrix");
		lightDirectionID = glGetUniformLocation(programID, "lightDirection");
		partColorID = glGetUniformLocation(programID, "partColor");
	}
	void setModelMatrix(Mat4f modelMatrix) {
		float modelData[16];
		modelMatrix.toColMajorData(modelData);
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, modelData);
	}
	void setPVMatrix(Mat4f pvMatrix) {
		float pvData[16];
		pvMatrix.toColMajorData(pvData);
		glUniformMatrix4fv(pvMatrixID, 1, GL_FALSE, pvData);
	}
	void setLightDirection(Vec3f lightDirection) {
		glUniform3fv(lightDirectionID, 1, lightDirection.data);
	}
	void setPartColor(Color color) {
		glUniform3fv(partColorID, 1, color.data);
	}
};

class ArrayMesh {
	GLuint vertexArrayID;
	GLuint vertexBuffer;
	GLuint normalsBuffer;

	size_t triangleCount;
public:
	ArrayMesh(const float* vertexData, const float* normalsData, size_t triangleCount) : triangleCount(triangleCount) {
		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		// benerate buffers
		GLuint buffers[2];
		glGenBuffers(2, buffers);
		vertexBuffer = buffers[0];
		normalsBuffer = buffers[1];

		// load vertices
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, triangleCount * 3 * 3 * sizeof(float), vertexData, GL_STATIC_DRAW);

		// load normals
		glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
		glBufferData(GL_ARRAY_BUFFER, triangleCount * 3 * 3 * sizeof(float), normalsData, GL_STATIC_DRAW);
	}
	void bind() const {
		// 1st attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : normals
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_TRUE,                          // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
	}
	void unbind() const {
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
	void render() const {
		glDrawArrays(GL_TRIANGLES, 0, 3 * triangleCount);
	}
};

class MeshRegistry {
	std::vector<const ShapeClass*> knownShapeClasses;
	std::vector<ArrayMesh> meshes;

	int findKnownShapeClassIndex(const ShapeClass* shapeClass) {
		for(size_t i = 0; i < knownShapeClasses.size(); i++) {
			if(knownShapeClasses[i] == shapeClass) {
				return static_cast<int>(i);
			}
		}
		return -1;
	}
public:
	int registerShapeClassMesh(const ShapeClass* shapeClass, ArrayMesh&& mesh) {
		knownShapeClasses.push_back(shapeClass);
		meshes.push_back(std::move(mesh));
		return static_cast<int>(meshes.size() - 1);
	}
	int getMeshIndexFor(const ShapeClass* shapeClass) {
		for(size_t i = 0; i < knownShapeClasses.size(); i++) {
			if(knownShapeClasses[i] == shapeClass) {
				return static_cast<int>(i);
			}
		}
		Polyhedron shapeClassPolyhedron = shapeClass->asPolyhedron();
		Vec3f* vertexBuf = new Vec3f[shapeClassPolyhedron.triangleCount * 3];
		Vec3f* normalBuf = new Vec3f[shapeClassPolyhedron.triangleCount * 3];
		for(int i = 0; i < shapeClassPolyhedron.triangleCount; i++) {
			Triangle t = shapeClassPolyhedron.getTriangle(i);
			vertexBuf[i*3+0] = shapeClassPolyhedron.getVertex(t.firstIndex);
			vertexBuf[i*3+1] = shapeClassPolyhedron.getVertex(t.secondIndex);
			vertexBuf[i*3+2] = shapeClassPolyhedron.getVertex(t.thirdIndex);

			Vec3f normal = normalize(shapeClassPolyhedron.getNormalVecOfTriangle(t));
			normalBuf[i*3+0] = normal;
			normalBuf[i*3+1] = normal;
			normalBuf[i*3+2] = normal;
		}
		ArrayMesh arMesh(reinterpret_cast<float*>(vertexBuf), reinterpret_cast<float*>(normalBuf), shapeClassPolyhedron.triangleCount);
		delete[] vertexBuf;
		delete[] normalBuf;
		return registerShapeClassMesh(shapeClass, std::move(arMesh));
	}
	const ArrayMesh& operator[](int meshIndex) const {
		return meshes[meshIndex];
	}
};

MeshRegistry meshRegistry;

class CustomPart : public Part {
public:
	int meshIndex;
	Color color;

	CustomPart(const Shape& shape, const GlobalCFrame& position, const PartProperties& properties, int meshIndex, Color color) : 
		Part(shape, position, properties), meshIndex(meshIndex), color(color) {}

	CustomPart(const Shape& shape, const GlobalCFrame& position, const PartProperties& properties, Color color) : 
		Part(shape, position, properties), meshIndex(meshRegistry.getMeshIndexFor(shape.baseShape)), color(color) {}
};

void render(SynchronizedWorld<CustomPart>& world, RenderShader& shader) {
	Mat4f viewMatrix = lookAt(Vec3f(12.0f, 6.0f, 15.0f), Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 1.0f, 0.0f));
	Mat4f projectionMatrix = perspective(0.8f, 4.0f/3.0f, 0.1f, 100.0f);

	shader.setPVMatrix(projectionMatrix * viewMatrix);

	// multithreaded application, must use synchronization with PhysicsThread
	world.syncReadOnlyOperation([&world, &shader]() {
		world.forEachPart([&](const CustomPart& part) {
			const ArrayMesh& mesh = meshRegistry[part.meshIndex];
			mesh.bind();

			shader.setPartColor(part.color);
			shader.setModelMatrix(part.getCFrame().asMat4WithPreScale(part.hitbox.scale));
			mesh.render();

			mesh.unbind();
		});
	});
}

int main(int argc, const char** argv) {
	std::cout << "Starting Physics3D Demo" << std::endl;
	GLFWwindow* window = initWindow();
	if(!window) return -1;

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Add face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderShader shader;

	// World constructor accepts the tick delay. This code creates a world to run at 100 TPS. 
	SynchronizedWorld<CustomPart> world(1 / 100.0);
	
	PartProperties basicProperties;
	basicProperties.density = 1.0;
	basicProperties.friction = 0.5;
	basicProperties.bouncyness = 0.4;
	
	// We must manage Part memory ourselves. World is simply a Part* container. 
	std::unique_ptr<CustomPart> floor = std::make_unique<CustomPart>(
		boxShape(20.0, 0.5, 20.0), 
		GlobalCFrame(0.0, -2.0, 0.0), 
		basicProperties, 
		Colors::WHITE);
	std::unique_ptr<CustomPart> wall = std::make_unique<CustomPart>(
		boxShape(3.0, 1.0, 10.0), 
		GlobalCFrame(7.0, -2.0, 0.0, Rotation::fromEulerAngles(0.0, 0.0, 0.2)), 
		basicProperties, 
		Colors::GRAY);
	std::unique_ptr<CustomPart> box = std::make_unique<CustomPart>(
		boxShape(2.0, 0.5, 3.0), 
		GlobalCFrame(0.0, 1.3, 1.0, Rotation::fromEulerAngles(0.2, 0.1, 0.3)), 
		basicProperties, 
		Colors::BLUE);
	std::unique_ptr<CustomPart> sphere = std::make_unique<CustomPart>(
		sphereShape(1.0), 
		GlobalCFrame(1.4, 4.7, 1.0), 
		basicProperties, 
		Colors::GREEN);
	std::unique_ptr<CustomPart> icosahedron = std::make_unique<CustomPart>(
		polyhedronShape(ShapeLibrary::icosahedron), 
		GlobalCFrame(4.0, 1.3, 4.0), 
		basicProperties, 
		Colors::RED);
	
	world.addTerrainPart(floor.get());
	world.addTerrainPart(wall.get());
	world.addPart(box.get());
	world.addPart(sphere.get());
	world.addPart(icosahedron.get());

	// We can set part velocities and other properties. 
	icosahedron->setAngularVelocity(Vec3(0.0, 10.0, 0.0));

	// External Force memory is also not managed by the engine. 
	std::unique_ptr<DirectionalGravity> gravity = std::make_unique<DirectionalGravity>(Vec3(0.0, -10.0, 0.0));
	world.addExternalForce(gravity.get());

	PhysicsThread physicsThread(&world);
	physicsThread.start();

	shader.setLightDirection(normalize(Vec3f(0.4f, -1.0f, 0.2f)));

	// render loop
	do {
		// Clear the screen. 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render(world, shader);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	
	// Check if the ESC key was pressed or the window was closed 
	} while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	return 0;
}
