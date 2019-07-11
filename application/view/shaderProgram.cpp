#include "shaderProgram.h"

#include <sstream>

#include "../resourceManager.h"

namespace Shaders {
	BasicShader basicShader;
	DepthShader depthShader;
	VectorShader vectorShader;
	OriginShader originShader;
	FontShader fontShader;
	QuadShader quadShader;
	PostProcessShader postProcessShader;
	SkyboxShader skyboxShader;
	PointShader pointShader;
	TestShader testShader;
	BlurShader blurShader;
	ColorWheelShader colorWheelShader;
	LineShader lineShader;

	void init() {
		// Shader source init
		ShaderSource basicShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(BASIC_SHADER)), "basic.shader");
		ShaderSource depthShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(DEPTH_SHADER)), "depth.shader");
		ShaderSource vectorShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(VECTOR_SHADER)), "vector.shader");
		ShaderSource fontShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(FONT_SHADER)), "font.shader");
		ShaderSource originShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(ORIGIN_SHADER)), "origin.shader");
		ShaderSource quadShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(QUAD_SHADER)), "quad.shader");
		ShaderSource postProcessShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(POSTPROCESS_SHADER)), "postProcess.shader");
		ShaderSource skyboxShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(SKYBOX_SHADER)), "skybox.shader");
		ShaderSource pointShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(POINT_SHADER)), "point.shader");
		ShaderSource testShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(TEST_SHADER)), "test.shader");
		ShaderSource blurShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(BLUR_SHADER)), "blur.shader");
		ShaderSource colorWheelShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(COLORWHEEL_SHADER)), "colorwheel.shader");
		ShaderSource lineShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(LINE_SHADER)), "line.shader");

		// Shader init
		new(&basicShader) BasicShader(basicShaderSource);
		new(&depthShader) DepthShader(depthShaderSource);
		new(&vectorShader) VectorShader(vectorShaderSource);
		new(&fontShader) FontShader(fontShaderSource);
		new(&originShader) OriginShader(originShaderSource);
		new(&quadShader) QuadShader(quadShaderSource);
		new(&postProcessShader) PostProcessShader(postProcessShaderSource);
		new(&skyboxShader) SkyboxShader(skyboxShaderSource);
		new(&pointShader) PointShader(pointShaderSource);
		new(&testShader) TestShader(testShaderSource);
		new(&blurShader) BlurShader(blurShaderSource);
		new(&colorWheelShader) ColorWheelShader(colorWheelShaderSource);
		new(&lineShader) LineShader(lineShaderSource);

		basicShader.createLightArray(4);
	}

	void close() {
		basicShader.close();
		depthShader.close();
		vectorShader.close();
		fontShader.close();
		originShader.close();
		skyboxShader.close();
		quadShader.close();
		postProcessShader.close();
		depthShader.close();
		skyboxShader.close();
		pointShader.close();
		testShader.close();
		blurShader.close();
		colorWheelShader.close();
		lineShader.close();
	}
}