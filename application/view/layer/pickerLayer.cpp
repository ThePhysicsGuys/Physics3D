#include "core.h"

#include "pickerLayer.h"

#include "../screen.h"

#include "../picker/picker.h"

#include "../../input/standardInputHandler.h"

PickerLayer::PickerLayer() : Layer() {

}

PickerLayer::PickerLayer(Screen * screen, char flags) : Layer("Picker", screen, flags) {

}

void PickerLayer::onInit() {
	Picker::onInit();
}

void PickerLayer::onUpdate() {
	Picker::onUpdate(*screen, handler->mousePosition);
}

void PickerLayer::onEvent(Event& event) {
	Picker::onEvent(event);
}

void PickerLayer::onRender() {
	Picker::onRender(*screen);
}

void PickerLayer::onClose() {
	Picker::onClose();
}
