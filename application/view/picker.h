#pragma once

#include "screen.h"
#include "indexedMesh.h"
#include "shaderProgram.h"

#include "../engine/math/vec2.h"

namespace Picker {

	enum class EditMode {
		TRANSLATE,
		ROTATE,
		SCALE
	};

	extern EditMode editMode;

	extern IndexedMesh* rotateX;
	extern IndexedMesh* rotateY;
	extern IndexedMesh* rotateZ;
	 
	extern IndexedMesh* translateCenter;
	extern IndexedMesh* translateX;
	extern IndexedMesh* translateY;
	extern IndexedMesh* translateZ;
	 
	extern IndexedMesh* scaleCenter;
	extern IndexedMesh* scaleX;
	extern IndexedMesh* scaleY;
	extern IndexedMesh* scaleZ;

	void init();
	void update(Screen& screen, Vec2 mousePosition);
	void render(Screen& screen, BasicShader& shader);

	void moveGrabbedPhysicalTransversal(Screen& screen, double dz);
	void moveGrabbedPhysicalLateral(Screen& screen);
}