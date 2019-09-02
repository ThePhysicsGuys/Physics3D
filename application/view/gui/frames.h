#pragma once

#include "frame.h"
#include "label.h"
#include "slider.h"
#include "checkBox.h"
#include "button.h"
#include "directionEditor.h"
#include "colorPicker.h"

#include "../engine/math/mathUtil.h"
#include "../debug/visualDebug.h"
#include "../../extendedPart.h"
#include "../shaderProgram.h"
#include "../renderUtils.h"
#include "../../worlds.h"
#include "../screen.h"

// Frame blueprint

struct FrameBlueprint {
	virtual void init() = 0;
	virtual void update() = 0;
};

// Environment frame

struct EnvironmentFrame : public FrameBlueprint, public Frame {

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

	EnvironmentFrame(double x, double y) : Frame(x, y, "Environment")  {
		// frame = new Frame(x, y, "Environment");

		init();

		add(hdrCheckBox, Align::FILL);
		add(gammaLabel, Align::CENTER);
		add(gammaSlider, Align::RELATIVE);
		add(gammaValueLabel, Align::FILL);
		add(exposureLabel, Align::CENTER);
		add(exposureSlider, Align::RELATIVE);
		add(exposureValueLabel, Align::FILL);
		add(sunLabel, Align::CENTER);
		add(sunColorButton, Align::CENTER);
		add(sunDirectionEditor, Align::CENTER);
		
		GUI::add(this);
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
			Shaders::basicShader.updateGamma(float(s->value));
		};
		gammaValueLabel = new Label("", 0, 0);

		exposureLabel = new Label("Exposure", 0, 0);
		exposureSlider = new Slider(0, 0, 0, 2, 1);
		exposureSlider->action = [] (Slider* s) {
			Shaders::basicShader.updateExposure(float(s->value));
		};
		exposureValueLabel = new Label("", 0, 0);

		sunLabel = new Label("Sun", 0, 0);
		sunColorButton = new Button(0, 0, GUI::sliderBarWidth, GUI::sliderHandleHeight, false);
		sunDirectionEditor = new DirectionEditor(0, 0, GUI::sliderBarWidth, GUI::sliderBarWidth);
		sunDirectionEditor->action = [] (DirectionEditor* d) {
			Shaders::basicShader.updateSunDirection(d->modelMatrix * Vec3(0, 1, 0));
		};

		sunColorButton->setColor(Vec4(1));
		sunColorButton->action = [] (Button* button) {
			EnvironmentFrame* environmentFrame = static_cast<EnvironmentFrame*>(button->parent);

			GUI::colorPickerFrame->visible = true;
			GUI::colorPickerFrame->anchor = environmentFrame;
			GUI::select(GUI::colorPickerFrame);

			GUI::colorPicker->setRgba(button->idleColor);
			GUI::colorPicker->focus = button;
			GUI::colorPicker->action = [] (ColorPicker* p) {
				Shaders::basicShader.updateSunColor(Vec3(p->getRgba()));
			};
		};
	}

	void update() override {
		if (!visible)
			return;

		if (hdrCheckBox->checked) {
			exposureSlider->enable();
			exposureValueLabel->enable();
			exposureLabel->enable();
			exposureValueLabel->text = std::to_string(exposureSlider->value);
		} else {
			exposureSlider->disable();
			exposureValueLabel->disable();
			exposureLabel->disable();
		}

		if (GUI::colorPicker->focus == sunColorButton) {
			sunColorButton->setColor(GUI::colorPicker->getRgba());
		}

		gammaValueLabel->text = std::to_string(gammaSlider->value);
	}
};


// Debug frame

struct DebugFrame : public FrameBlueprint, public Frame {

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

	DebugFrame(double x, double y) : Frame(x, y, "Debug") {

		init();

		add(vectorLabel, Align::CENTER);
		add(infoVectorCheckBox, Align::FILL);
		add(positionCheckBox, Align::FILL);
		add(velocityCheckBox, Align::FILL);
		add(accelerationCheckBox, Align::FILL);
		add(forceCheckBox, Align::FILL);
		add(momentCheckBox, Align::FILL);
		add(impulseCheckBox, Align::FILL);
		add(angularVelocityCheckBox, Align::FILL);
		add(angularImpulseCheckBox, Align::FILL);
		add(pointLabel, Align::CENTER);
		add(infoPointCheckBox, Align::FILL);
		add(centerOfMassCheckBox, Align::FILL);
		add(intersectionCheckBox, Align::FILL);
		add(renderLabel, Align::CENTER);
		add(renderPiesCheckBox, Align::FILL);
		add(renderSpheresCheckBox, Align::FILL);

		GUI::add(this);
	}

	void init() override {
		visible = false;

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
		infoVectorCheckBox->action = [](CheckBox* c) { toggleVectorType(Debug::INFO_VEC); };
		velocityCheckBox->action = [](CheckBox* c) { toggleVectorType(Debug::VELOCITY); };
		accelerationCheckBox->action = [](CheckBox* c) { toggleVectorType(Debug::ACCELERATION); };
		forceCheckBox->action = [](CheckBox* c) { toggleVectorType(Debug::FORCE); };
		angularImpulseCheckBox->action = [](CheckBox* c) { toggleVectorType(Debug::ANGULAR_IMPULSE); };
		positionCheckBox->action = [](CheckBox* c) { toggleVectorType(Debug::POSITION); };
		momentCheckBox->action = [](CheckBox* c) { toggleVectorType(Debug::MOMENT); };
		impulseCheckBox->action = [](CheckBox* c) { toggleVectorType(Debug::IMPULSE); };
		angularVelocityCheckBox->action = [](CheckBox* c) { toggleVectorType(Debug::ANGULAR_VELOCITY); };
		infoPointCheckBox->action = [](CheckBox* c) { togglePointType(Debug::INFO_POINT); };
		centerOfMassCheckBox->action = [](CheckBox* c) { togglePointType(Debug::CENTER_OF_MASS); };
		intersectionCheckBox->action = [](CheckBox* c) { togglePointType(Debug::INTERSECTION); };
		renderPiesCheckBox->action = [](CheckBox* c) { Debug::renderPiesEnabled = !Debug::renderPiesEnabled; };
		renderSpheresCheckBox->action = [](CheckBox* c) { Debug::colissionSpheresMode = static_cast<Debug::SphereColissionRenderMode>((static_cast<int>(Debug::colissionSpheresMode) + 1) % 3); };
	}

	void update() override {
		if (!visible)
			return;

		infoVectorCheckBox->checked = Debug::vectorDebugEnabled[Debug::INFO_VEC];
		positionCheckBox->checked = Debug::vectorDebugEnabled[Debug::POSITION];
		velocityCheckBox->checked = Debug::vectorDebugEnabled[Debug::VELOCITY];
		momentCheckBox->checked = Debug::vectorDebugEnabled[Debug::MOMENT];
		forceCheckBox->checked = Debug::vectorDebugEnabled[Debug::FORCE];
		accelerationCheckBox->checked = Debug::vectorDebugEnabled[Debug::ACCELERATION];
		angularImpulseCheckBox->checked = Debug::vectorDebugEnabled[Debug::ANGULAR_IMPULSE];
		impulseCheckBox->checked = Debug::vectorDebugEnabled[Debug::IMPULSE];
		angularVelocityCheckBox->checked = Debug::vectorDebugEnabled[Debug::ANGULAR_VELOCITY];
		infoPointCheckBox->checked = Debug::pointDebugEnabled[Debug::INFO_POINT];
		centerOfMassCheckBox->checked = Debug::pointDebugEnabled[Debug::CENTER_OF_MASS];
		intersectionCheckBox->checked = Debug::pointDebugEnabled[Debug::INTERSECTION];
		renderPiesCheckBox->checked = Debug::renderPiesEnabled;
		renderSpheresCheckBox->checked = Debug::colissionSpheresMode != Debug::SphereColissionRenderMode::NONE;
	}

};


// Properties frame

struct PropertiesFrame : public FrameBlueprint, public Frame {

	Label* generalLabel = nullptr;
	Label* partNameLabel = nullptr;
	Label* partMeshIDLabel = nullptr;

	Label* physicalLabel = nullptr;
	Label* positionLabel = nullptr;
	Label* velocityLabel = nullptr;
	Label* angularVelocityLabel = nullptr;
	Label* kineticEnergyLabel = nullptr;
	Label* potentialEnergyLabel = nullptr;
	Label* energyLabel = nullptr;
	Label* massLabel = nullptr;
	Label* frictionLabel = nullptr;
	Label* densityLabel = nullptr;

	Label* materialLabel = nullptr;
	Button* ambientColorButton = nullptr;
	Button* diffuseColorButton = nullptr;
	Button* specularColorButton = nullptr;
	Slider* reflectanceSlider = nullptr;
	CheckBox* renderModeCheckBox = nullptr;

	PropertiesFrame(double x, double y) : Frame(x, y, "Properties") {
		init();
	
		add(generalLabel, Align::CENTER);
		add(partNameLabel, Align::FILL);
		add(partMeshIDLabel, Align::FILL);

		add(physicalLabel, Align::CENTER);
		add(positionLabel, Align::FILL);
		add(velocityLabel, Align::FILL);
		add(angularVelocityLabel, Align::FILL);
		add(kineticEnergyLabel, Align::FILL);
		add(potentialEnergyLabel, Align::FILL);
		add(energyLabel, Align::FILL);
		add(massLabel, Align::FILL);
		add(frictionLabel, Align::FILL);
		add(densityLabel, Align::FILL);

		add(materialLabel, Align::CENTER);
		add(ambientColorButton, Align::FILL);
		add(diffuseColorButton, Align::FILL);
		add(specularColorButton, Align::FILL);
		add(reflectanceSlider, Align::FILL);
		add(renderModeCheckBox, Align::FILL);

		GUI::add(this);
	}

	void init() override {
		generalLabel = new Label("General", 0, 0);
		partNameLabel = new Label("", 0, 0);
		partMeshIDLabel = new Label("", 0, 0);

		physicalLabel = new Label("Physical", 0, 0);
		positionLabel = new Label("", 0, 0);
		velocityLabel = new Label("", 0, 0);
		angularVelocityLabel = new Label("", 0, 0);
		kineticEnergyLabel = new Label("", 0, 0);
		potentialEnergyLabel = new Label("", 0, 0);
		energyLabel = new Label("", 0, 0);
		massLabel = new Label("", 0, 0);
		frictionLabel = new Label("", 0, 0);
		densityLabel = new Label("", 0, 0);

		materialLabel = new Label("Material", 0, 0);
		renderModeCheckBox = new CheckBox("Wireframe", 0, 0, true);
		renderModeCheckBox->action = [] (CheckBox* c) {
			if (GUI::screen->selectedPart) {
				if (GUI::screen->selectedPart->renderMode == Renderer::FILL) {
					GUI::screen->selectedPart->renderMode = Renderer::WIREFRAME;
				} else {
					GUI::screen->selectedPart->renderMode = Renderer::FILL;
				}
			}
		};

		ambientColorButton = new Button(0, 0, GUI::sliderBarWidth, GUI::sliderHandleHeight, false);
		ambientColorButton->action = [] (Button* button) {
			PropertiesFrame* propertiesFrame = static_cast<PropertiesFrame*>(button->parent);

			if (GUI::screen->selectedPart) {
				GUI::colorPicker->focus = button;
				GUI::select(GUI::colorPickerFrame);
				GUI::colorPickerFrame->visible = true;
				GUI::colorPickerFrame->anchor = propertiesFrame;
				GUI::colorPicker->action = [] (ColorPicker* colorPicker) {
					GUI::screen->selectedPart->material.ambient = colorPicker->getRgba();
				};

			}
		};

		diffuseColorButton = new Button(0, 0, GUI::sliderBarWidth, GUI::sliderHandleHeight, false);
		diffuseColorButton->action = [] (Button* button) {
			PropertiesFrame* propertiesFrame = static_cast<PropertiesFrame*>(button->parent);
			if (GUI::screen->selectedPart) {
				GUI::colorPicker->focus = button;
				GUI::select(GUI::colorPickerFrame);
				GUI::colorPickerFrame->visible = true;
				GUI::colorPickerFrame->anchor = propertiesFrame;
				GUI::colorPicker->action = [] (ColorPicker* colorPicker) {
					GUI::screen->selectedPart->material.diffuse = Vec3(colorPicker->getRgba());
				};
			}
		};

		specularColorButton = new Button(0, 0, GUI::sliderBarWidth, GUI::sliderHandleHeight, false);
		specularColorButton->action = [] (Button* button) {
			PropertiesFrame* propertiesFrame = static_cast<PropertiesFrame*>(button->parent);
			if (GUI::screen->selectedPart) {
				GUI::colorPicker->focus = button;
				GUI::select(GUI::colorPickerFrame);
				GUI::colorPickerFrame->visible = true;
				GUI::colorPickerFrame->anchor = propertiesFrame;
				GUI::colorPicker->action = [] (ColorPicker* colorPicker) {
					GUI::screen->selectedPart->material.specular = Vec3(colorPicker->getRgba());
				};
			}
		};

		reflectanceSlider = new Slider(0, 0, 0, 5, 1);
		reflectanceSlider->action = [] (Slider* slider) {
			if (GUI::screen->selectedPart) {
				GUI::screen->selectedPart->material.reflectance = static_cast<float>(slider->value);
			}
		};
	}

	void update() override {
		if (!visible)
			return;

		ExtendedPart* selectedPart = GUI::screen->selectedPart;
		MagnetWorld* world = GUI::screen->world;

		if (selectedPart) {
			partMeshIDLabel->text = "MeshID: " + std::to_string(selectedPart->drawMeshId);

			positionLabel->text = "Position: " + str(selectedPart->getCFrame().position);
			partNameLabel->text = "Name: " + selectedPart->name;
			velocityLabel->text = "Velocity: " + str(selectedPart->parent->velocity);
			angularVelocityLabel->text = "Angular Velocity: " + str(selectedPart->parent->angularVelocity);
			double kineticEnergy = selectedPart->parent->getKineticEnergy();
			double potentialEnergy = world->getPotentialEnergyOfPhysical(*selectedPart->parent);
			kineticEnergyLabel->text = "Kinetic Energy: " + std::to_string(kineticEnergy);
			potentialEnergyLabel->text = "Potential Energy: " + std::to_string(potentialEnergy);
			energyLabel->text = "Energy: " + std::to_string(kineticEnergy + potentialEnergy);
			massLabel->text = "Mass: " + std::to_string(selectedPart->mass);
			frictionLabel->text = "Friction: " + std::to_string(selectedPart->properties.friction);
			densityLabel->text = "Density: " + std::to_string(selectedPart->properties.density);

			renderModeCheckBox->checked = selectedPart->renderMode == Renderer::WIREFRAME;

			ambientColorButton->disabled = false;
			ambientColorButton->setColor(selectedPart->material.ambient);
			if (GUI::colorPicker->focus == ambientColorButton) {
				GUI::colorPicker->setRgba(selectedPart->material.ambient);
			}

			diffuseColorButton->disabled = false;
			diffuseColorButton->setColor(Vec4(selectedPart->material.diffuse, 1));
			if (GUI::colorPicker->focus == diffuseColorButton) {
				GUI::colorPicker->setRgba(Vec4(selectedPart->material.diffuse, 1));
			}

			specularColorButton->disabled = false;
			specularColorButton->setColor(Vec4(selectedPart->material.specular, 1));
			if (GUI::colorPicker->focus == specularColorButton) {
				GUI::colorPicker->setRgba(Vec4(selectedPart->material.specular, 1));
			}

			reflectanceSlider->disabled = false;
			reflectanceSlider->value = selectedPart->material.reflectance;
		} else {
			partMeshIDLabel->text = "MeshID: -";

			positionLabel->text = "Position: -";
			partNameLabel->text = "Name: -";
			velocityLabel->text = "Velocity: -";
			angularVelocityLabel->text = "Angular Velocity: -";
			kineticEnergyLabel->text = "Kinetic Energy: -";
			potentialEnergyLabel->text = "Potential Energy: -";
			energyLabel->text = "Energy: -";
			massLabel->text = "Mass: -";
			frictionLabel->text = "Friction: -";
			densityLabel->text = "Density: -";

			ambientColorButton->disabled = true;
			ambientColorButton->setColor(Vec4(1));

			diffuseColorButton->disabled = true;
			diffuseColorButton->setColor(Vec4(1));

			specularColorButton->disabled = true;
			specularColorButton->setColor(Vec4(1));

			reflectanceSlider->disabled = true;
			reflectanceSlider->value = (reflectanceSlider->max + reflectanceSlider->min) / 2;

			renderModeCheckBox->checked = false;
		}
	}

};