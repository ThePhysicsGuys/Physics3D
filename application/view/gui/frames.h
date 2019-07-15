#pragma once

#include "frame.h"
#include "label.h"
#include "slider.h"
#include "checkBox.h"
#include "directionEditor.h"
#include "colorPicker.h"

#include "../visualDebug.h"
#include "../../extendedPart.h"
#include "../../worlds.h"

// Frame blueprint

struct FrameBlueprint {
	Frame* frame = nullptr;

	virtual void init() = 0;
	virtual void update() = 0;
};

// Environment frame

struct EnvironmentFrame : public FrameBlueprint {

	Label* gammaLabel = nullptr;
	Slider* gammaSlider = nullptr;
	Label* gammaValueLabel = nullptr;

	Label* exposureLabel = nullptr;
	Slider* exposureSlider = nullptr;
	Label* exposureValueLabel = nullptr;
	CheckBox* hdrCheckBox = nullptr;

	Label* sunLabel = nullptr;
	Button* sunColorButton = nullptr;
	DirectionEditor* sunDirectionEditor = nullptr;

	Frame* colorPickerFrame = nullptr;
	ColorPicker* colorPicker = nullptr;

	EnvironmentFrame(double x, double y)  {
		frame = new Frame(x, y, "Environment");

		init();

		frame->add(hdrCheckBox, Align::FILL);
		frame->add(gammaLabel, Align::CENTER);
		frame->add(gammaSlider, Align::RELATIVE);
		frame->add(gammaValueLabel, Align::FILL);
		frame->add(exposureLabel, Align::CENTER);
		frame->add(exposureSlider, Align::RELATIVE);
		frame->add(exposureValueLabel, Align::FILL);
		frame->add(sunLabel, Align::CENTER);
		frame->add(sunColorButton, Align::CENTER);
		frame->add(sunDirectionEditor, Align::CENTER);
		
		colorPickerFrame->add(colorPicker, Align::FILL);

		GUI::add(colorPickerFrame);
		GUI::add(frame);
	}

	void init() override {
		hdrCheckBox = new CheckBox("HDR", 0, 0, true);
		hdrCheckBox->checked = true;
		hdrCheckBox->action = [](CheckBox* c) {
			Shaders::basicShader.updateHDR(c->checked);
		};

		gammaLabel = new Label("Gamma", 0, 0);
		gammaSlider = new Slider(0, 0, 0, 3, 1);
		gammaSlider->action = [] (Slider* s) {
			Shaders::basicShader.updateGamma(s->value);
		};
		gammaValueLabel = new Label("", 0, 0);

		exposureLabel = new Label("Exposure", 0, 0);
		exposureSlider = new Slider(0, 0, 0, 2, 1);
		exposureSlider->action = [] (Slider* s) {
			Shaders::basicShader.updateExposure(s->value);
		};
		exposureValueLabel = new Label("", 0, 0);

		sunLabel = new Label("Sun", 0, 0);
		sunColorButton = new Button(0, 0, GUI::sliderBarWidth, GUI::sliderHandleHeight, false);
		sunDirectionEditor = new DirectionEditor(0, 0, GUI::sliderBarWidth, GUI::sliderBarWidth);
		sunDirectionEditor->action = [] (DirectionEditor* d) {
			Shaders::basicShader.updateSunDirection(d->modelMatrix * Vec3(0, 1, 0));
		};


		// Colorpicker GUI
		colorPickerFrame = new Frame(0, 0, "Color");
		colorPickerFrame->visible = false;

		colorPicker = new ColorPicker(0, 0, 0.5);
		colorPickerFrame->anchor = frame;

		sunColorButton->action = [] (Button* c) {
			Frame* colorPickerFrame = static_cast<Frame*>(c->parent);
			if (!colorPickerFrame->visible) {
				colorPickerFrame->visible = true;
				colorPickerFrame->position = Vec2(0);
				GUI::select(colorPickerFrame);

				// TODO fix anchoring
				// colorPickerFrame->anchor = frame;
			}
		};

		colorPicker->action = [] (ColorPicker* c) {
			Shaders::basicShader.updateSunColor(Vec3(c->getRgba()));
		};
	}

	void update() override {
		if (!frame->visible)
			return;

		Vec4 color = colorPicker->getRgba();
		sunColorButton->idleColor = color;
		sunColorButton->hoverColor = color;
		sunColorButton->pressColor = color;

		if (hdrCheckBox->checked) {
			exposureSlider->disabled = false;
			exposureValueLabel->disabled = false;
			exposureLabel->disabled = false;
			exposureValueLabel->text = std::to_string(exposureSlider->value);
		} else {
			exposureSlider->disabled = true;
			exposureValueLabel->disabled = true;
			exposureLabel->disabled = true;
		}

		gammaValueLabel->text = std::to_string(gammaSlider->value);
	}
};


// Debug frame

struct DebugFrame : public FrameBlueprint {

	Label* vectorLabel = nullptr;
	CheckBox* infoVectorCheckBox = nullptr;
	CheckBox* positionCheckBox = nullptr;
	CheckBox* velocityCheckBox = nullptr;
	CheckBox* momentCheckBox = nullptr;
	CheckBox* forceCheckBox = nullptr;
	CheckBox* accelerationCheckBox = nullptr;
	CheckBox* angularImpulseCheckBox = nullptr;
	CheckBox* impulseCheckBox = nullptr;
	CheckBox* angularVelocityCheckBox = nullptr;
	Label* pointLabel = nullptr;
	CheckBox* infoPointCheckBox = nullptr;
	CheckBox* centerOfMassCheckBox = nullptr;
	CheckBox* intersectionCheckBox = nullptr;
	Label* renderLabel = nullptr;
	CheckBox* renderPiesCheckBox = nullptr;
	CheckBox* renderSpheresCheckBox = nullptr;

	DebugFrame(double x, double y) {
		frame = new Frame(x, y, "Debug");

		init();

		frame->add(vectorLabel, Align::CENTER);
		frame->add(infoVectorCheckBox, Align::FILL);
		frame->add(positionCheckBox, Align::FILL);
		frame->add(velocityCheckBox, Align::FILL);
		frame->add(accelerationCheckBox, Align::FILL);
		frame->add(forceCheckBox, Align::FILL);
		frame->add(momentCheckBox, Align::FILL);
		frame->add(impulseCheckBox, Align::FILL);
		frame->add(angularVelocityCheckBox, Align::FILL);
		frame->add(angularImpulseCheckBox, Align::FILL);
		frame->add(pointLabel, Align::CENTER);
		frame->add(infoPointCheckBox, Align::FILL);
		frame->add(centerOfMassCheckBox, Align::FILL);
		frame->add(intersectionCheckBox, Align::FILL);
		frame->add(renderLabel, Align::CENTER);
		frame->add(renderPiesCheckBox, Align::FILL);
		frame->add(renderSpheresCheckBox, Align::FILL);

		GUI::add(frame);
	}

	void init() override {
		frame->visible = false;

		vectorLabel = new Label("Vectors", 0, 0);
		infoVectorCheckBox = new CheckBox("Info", 0, 0, true);
		positionCheckBox = new CheckBox("Position", 0, 0, true);
		velocityCheckBox = new CheckBox("Velocity", 0, 0, true);
		accelerationCheckBox = new CheckBox("Acceleration", 0, 0, true);
		forceCheckBox = new CheckBox("Force", 0, 0, true);
		momentCheckBox = new CheckBox("Moment", 0, 0, true);
		impulseCheckBox = new CheckBox("Impulse", 0, 0, true);
		angularVelocityCheckBox = new CheckBox("Angular velocity", 0, 0, true);
		angularImpulseCheckBox = new CheckBox("Angular impulse", 0, 0, true);
		pointLabel = new Label("Points", 0, 0);
		infoPointCheckBox = new CheckBox("Info", 0, 0, true);
		centerOfMassCheckBox = new CheckBox("Center of mass", 0, 0, true);
		intersectionCheckBox = new CheckBox("Intersections", 0, 0, true);
		renderLabel = new Label("Render", 0, 0);
		renderPiesCheckBox = new CheckBox("Statistics", 0, 0, true);
		renderSpheresCheckBox = new CheckBox("Collision spheres", 0, 0, true);
		infoVectorCheckBox->action = [](CheckBox* c) { toggleDebugVecType(Debug::INFO_VEC); };
		velocityCheckBox->action = [](CheckBox* c) { toggleDebugVecType(Debug::VELOCITY); };
		accelerationCheckBox->action = [](CheckBox* c) { toggleDebugVecType(Debug::ACCELERATION); };
		forceCheckBox->action = [](CheckBox* c) { toggleDebugVecType(Debug::FORCE); };
		angularImpulseCheckBox->action = [](CheckBox* c) { toggleDebugVecType(Debug::ANGULAR_IMPULSE); };
		positionCheckBox->action = [](CheckBox* c) { toggleDebugVecType(Debug::POSITION); };
		momentCheckBox->action = [](CheckBox* c) { toggleDebugVecType(Debug::MOMENT); };
		impulseCheckBox->action = [](CheckBox* c) { toggleDebugVecType(Debug::IMPULSE); };
		angularVelocityCheckBox->action = [](CheckBox* c) { toggleDebugVecType(Debug::ANGULAR_VELOCITY); };
		infoPointCheckBox->action = [](CheckBox* c) { toggleDebugPointType(Debug::INFO_POINT); };
		centerOfMassCheckBox->action = [](CheckBox* c) { toggleDebugPointType(Debug::CENTER_OF_MASS); };
		intersectionCheckBox->action = [](CheckBox* c) { toggleDebugPointType(Debug::INTERSECTION); };
		renderPiesCheckBox->action = [](CheckBox* c) { renderPiesEnabled = !renderPiesEnabled; };
		renderSpheresCheckBox->action = [](CheckBox* c) { colissionSpheresMode = static_cast<SphereColissionRenderMode>((static_cast<int>(colissionSpheresMode) + 1) % 3); };
	}

	void update() override {
		if (!frame->visible)
			return;

		infoVectorCheckBox->checked = debug_enabled[Debug::INFO_VEC];
		positionCheckBox->checked = debug_enabled[Debug::POSITION];
		velocityCheckBox->checked = debug_enabled[Debug::VELOCITY];
		momentCheckBox->checked = debug_enabled[Debug::MOMENT];
		forceCheckBox->checked = debug_enabled[Debug::FORCE];
		accelerationCheckBox->checked = debug_enabled[Debug::ACCELERATION];
		angularImpulseCheckBox->checked = debug_enabled[Debug::ANGULAR_IMPULSE];
		impulseCheckBox->checked = debug_enabled[Debug::IMPULSE];
		angularVelocityCheckBox->checked = debug_enabled[Debug::ANGULAR_VELOCITY];
		infoPointCheckBox->checked = point_debug_enabled[Debug::INFO_POINT];
		centerOfMassCheckBox->checked = point_debug_enabled[Debug::CENTER_OF_MASS];
		intersectionCheckBox->checked = point_debug_enabled[Debug::INTERSECTION];
		renderPiesCheckBox->checked = renderPiesEnabled;
		renderSpheresCheckBox->checked = colissionSpheresMode != SphereColissionRenderMode::NONE;
	}

};


// Properties frame

struct PropertiesFrame : public FrameBlueprint {

	Label* partNameLabel = nullptr;
	Label* partPositionLabel = nullptr;
	Label* partMeshIDLabel = nullptr;
	Label* partVelocity = nullptr;
	Label* partAngularVelocity = nullptr;
	Label* partKineticEnergy = nullptr;
	Label* partPotentialEnergy = nullptr;
	Label* partEnergy = nullptr;
	Button* colorButton;
	CheckBox* renderModeCheckBox = nullptr;

	Frame* colorPickerFrame = nullptr;
	ColorPicker* colorPicker = nullptr;

	PropertiesFrame(double x, double y) {
		frame = new Frame(x, y, "Properties");

		init();
	
		frame->add(partNameLabel, Align::FILL);
		frame->add(partPositionLabel, Align::FILL);
		frame->add(partMeshIDLabel, Align::FILL);
		frame->add(renderModeCheckBox, Align::FILL);
		frame->add(colorButton, Align::FILL);
		frame->add(partVelocity, Align::FILL);
		frame->add(partAngularVelocity, Align::FILL);
		frame->add(partKineticEnergy, Align::FILL);
		frame->add(partPotentialEnergy, Align::FILL);
		frame->add(partEnergy, Align::FILL);

		colorPickerFrame->add(colorPicker, Align::FILL);

		GUI::add(colorPickerFrame);
		GUI::add(frame);
	}

	void init() override {
		partNameLabel = new Label("", 0, 0);
		partPositionLabel = new Label("", 0, 0);
		partMeshIDLabel = new Label("", 0, 0);
		colorButton = new Button(0, 0, GUI::sliderBarWidth, GUI::sliderHandleHeight, false);
		partVelocity = new Label("", 0, 0);
		partAngularVelocity = new Label("", 0, 0);
		partKineticEnergy = new Label("", 0, 0);
		partPotentialEnergy = new Label("", 0, 0);
		partEnergy = new Label("", 0, 0);
		renderModeCheckBox = new CheckBox("Wireframe", 0, 0, true);
		renderModeCheckBox->action = [] (CheckBox* c) {
			if (GUI::screen->selectedPart) {
				if (GUI::screen->selectedPart->renderMode == Renderer::FILLED) {
					GUI::screen->selectedPart->renderMode = Renderer::WIREFRAME;
				} else {
					GUI::screen->selectedPart->renderMode = Renderer::FILLED;
				}
			}
		};

		// Colorpicker GUI
		colorPickerFrame = new Frame(0, 0, "Color");
		colorPickerFrame->visible = false;

		colorPicker = new ColorPicker(0, 0, 0.5);
		colorPickerFrame->anchor = frame;

		colorButton->action = [] (Button* c) {
			Frame* colorPickerFrame = static_cast<Frame*>(c->parent);

			if (GUI::screen->selectedPart && !colorPickerFrame->visible) {
				colorPickerFrame->visible = true;
				colorPickerFrame->position = Vec2(0);
				//GUI::select(colorPickerFrame);

				// TODO fix anchoring
				//colorPickerFrame->anchor = propertiesFrame->frame;
			}
		};

		colorPicker->action = [] (ColorPicker* c) {
			if (GUI::screen->selectedPart) {
				GUI::screen->selectedPart->material.ambient = GUI::COLOR::hsvaToRgba(c->hsva);
			}
		};
	}

	void update() override {
		if (!frame->visible)
			return;

		ExtendedPart* selectedPart = GUI::screen->selectedPart;
		MagnetWorld* world = GUI::screen->world;

		if (GUI::screen->selectedPart) {
			partMeshIDLabel->text = "MeshID: " + std::to_string(selectedPart->drawMeshId);
			renderModeCheckBox->checked = selectedPart->renderMode == Renderer::WIREFRAME;
			partPositionLabel->text = "Position: " + str(selectedPart->cframe.position);
			partNameLabel->text = "Name: " + selectedPart->name;
			partVelocity->text = "Velocity: " + str(selectedPart->parent->velocity);
			partAngularVelocity->text = "Angular Velocity: " + str(selectedPart->parent->angularVelocity);
			double kineticEnergy = selectedPart->parent->getKineticEnergy();
			double potentialEnergy = world->getPotentialEnergyOfPhysical(*selectedPart->parent);
			partKineticEnergy->text = "Kinetic Energy: " + std::to_string(kineticEnergy);
			partPotentialEnergy->text = "Potential Energy: " + std::to_string(potentialEnergy);
			partEnergy->text = "Energy: " + std::to_string(kineticEnergy + potentialEnergy);

			Vec4 color = selectedPart->material.ambient;
			colorButton->idleColor = color;
			colorButton->hoverColor = color;
			colorButton->pressColor = color;
			colorPicker->setRgba(color);
		} else {
			colorButton->idleColor = Vec4(1);
			colorButton->hoverColor = Vec4(1);
			colorButton->pressColor = Vec4(1);
			partMeshIDLabel->text = "MeshID: -";
			renderModeCheckBox->checked = false;
			partPositionLabel->text = "Position: -";
			partNameLabel->text = "Name: -";
			partVelocity->text = "Velocity: -";
			partAngularVelocity->text = "Angular Velocity: -";
			partKineticEnergy->text = "Kinetic Energy: -";
			partPotentialEnergy->text = "Potential Energy: -";
			partEnergy->text = "Energy: -";
		}
	}

};