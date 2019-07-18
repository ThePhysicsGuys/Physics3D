#pragma once

#include "pickable.h"

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

	void init() override;
	void render(Screen& screen) override;

	float intersect(Screen& screen, const Ray& ray) override;

	void press(Screen& screen) override;
	void release(Screen& screen) override;
	void drag(Screen& screen) override;

	void dragTranslateTool(Screen& screen);
	void dragRotateTool(Screen& screen);
	void dragScaleTool(Screen& screen);
};

