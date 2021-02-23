#pragma once

#include "pickable.h"
#include "ecs/components.h"

namespace P3D::Application {

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
	Vec3 selectedPoint;

	// Intersected tool
	EditDirection intersectedEditDirection;
	Vec3 intersectedPoint;

	void onInit() override;
	void onRender(Screen& screen) override;
	void onClose() override;

	float intersect(Screen& screen, const Ray& ray) override;

	void onMousePress(Screen& screen) override;
	void onMouseRelease(Screen& screen) override;
	void onMouseDrag(Screen& screen) override;

	void dragTranslateTool(Screen& screen, Ref<Comp::Transform> transform);
	void dragRotateTool(Screen& screen, Ref<Comp::Transform> transform);
	void dragScaleTool(Screen& screen, Ref<Comp::Transform> transform);
};

};