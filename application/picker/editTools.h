#pragma once

#include "pickable.h"

namespace Application {

struct EditTools : public Pickable {

	enum class EditMode {
		TRANSLATE,
		ROTATE,
		SCALE
	};

	enum class EditDirection {
		NONE = -1,
		Y = 0,
		X = 1,
		Z = 2,
		CENTER = 3
	};

	// Current edit mode
	EditMode editMode = EditMode::TRANSLATE;

	// Selected tool
	EditDirection selectedEditDirection;
	Vec3f selectedPoint;

	// Intersected tool
	EditDirection intersectedEditDirection;
	Vec3f intersectedPoint;

	void onInit() override;
	void onRender(Screen& screen) override;
	void onClose();

	float intersect(Screen& screen, const Ray& ray) override;

	void onMousePress(Screen& screen) override;
	void onMouseRelease(Screen& screen) override;
	void onMouseDrag(Screen& screen) override;

	void dragTranslateTool(Screen& screen);
	void dragRotateTool(Screen& screen);
	void dragScaleTool(Screen& screen);
};

};