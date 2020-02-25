#pragma once

#include "../engine/layer/layer.h"
#include "../engine/visualData.h"

namespace Application {

class Screen;

class ConstraintLayer : public Layer {
	Screen* screen;


public:
	VisualData hexagon;
	ConstraintLayer() = default;
	inline ConstraintLayer(Screen* screen, char flags = NoUpdate | NoEvents) : screen(screen), Layer("Constraint layer", screen, flags) {}

	virtual void onAttach() override;
	virtual void onDetach() override;

	virtual void onInit() override;
	virtual void onUpdate() override;
	virtual void onRender() override;
	virtual void onClose() override;

	virtual void onEvent(Event& event) override;
};
};
