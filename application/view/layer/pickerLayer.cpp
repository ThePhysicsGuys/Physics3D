#include "core.h"

#include "pickerLayer.h"

#include "../screen.h"

#include "../picker/picker.h"

#include "../../input/standardInputHandler.h"

PickerLayer::PickerLayer() : Layer() {

}

PickerLayer::PickerLayer(Screen * screen, char flags) : Layer("Picker", screen, flags) {

}

void PickerLayer::init() {
	Picker::init();
}

void PickerLayer::update() {
	Picker::update(*screen, handler->cursorPosition);
}

void PickerLayer::event(Event& event) {

}

void PickerLayer::render() {
	Picker::render(*screen);
}

void PickerLayer::close() {
	Picker::close();
}
