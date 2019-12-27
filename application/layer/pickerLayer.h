#pragma once

#include "../engine/layer/layer.h"

namespace Application {

class PickerLayer : public Layer {
public:
	PickerLayer();
	PickerLayer(Screen* screen, char flags = None);

	void onInit() override;
	void onUpdate() override;
	void onEvent(Event& event) override;
	void onRender() override;
	void onClose() override;
};

};