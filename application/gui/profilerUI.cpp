
#include "GL\glew.h"
#include "profilerUI.h"
#include "../engine/math/mat2.h"
#include "../engine/math/vec2.h"

#define _USE_MATH_DEFINES
#include "math.h"

#define MAX_ANGLE 0.1f

const Vec3f pieColors[30]{
	Vec3f(1,0,0),
	Vec3f(0,1,0),
	Vec3f(0.2f,0.2f,1),
	Vec3f(1,1,0),
	Vec3f(1,0,1),
	Vec3f(0,1,1),
	Vec3f(1,1,1),
	Vec3f(0.5f,0,0),
	Vec3f(0,0.5f,0),
	Vec3f(0,0,0.5f),
	Vec3f(0.5f,0.5f,0),
	Vec3f(0.5f,0,0.5f),
	Vec3f(0,0.5f,0.5f),
	Vec3f(0.5f,0.5f,0.5f),
};

void vertex(Vec2f v) {
	glVertex2f(v.x, v.y);
}
void color(Vec3f c) {
	glColor3f(c.x, c.y, c.z);
}

void PieChart::renderPie(Screen& screen) const {
	Vec2f curPos = Vec2f(pieSize, 0);

	float totalWeight = getTotal();
	for(PiePart p : parts) {
		float angle = M_PI * 2 * p.weight / totalWeight;

		int subdivisions = angle / MAX_ANGLE + 1;
		
		Mat2f rotation = fromAngle(angle / subdivisions);

		glBegin(GL_TRIANGLE_FAN); {
			color(p.color);
			vertex(piePosition);
			vertex(piePosition + curPos);
			for(int i = 0; i < subdivisions; i++) {
				curPos = rotation * curPos;
				vertex(piePosition + curPos);
			}
		}glEnd();
	}
}

void PieChart::renderValueUnit(Font* font, float value, const char* unit, Vec2 position, Vec3f color) const {
	std::stringstream ss;
	ss.precision(4);
	ss << value;
	ss << unit;
	font->render(ss.str(), position, color, 0.0006);
}

void PieChart::renderText(Screen& screen, Font* font) const {
	// Title
	Vec2f titlePosition = piePosition + Vec2f(pieSize * 1.3f, pieSize * 1.1f);
	font->render(title, Vec2(titlePosition.x, titlePosition.y), Vec3f(1, 1, 1), 0.001);

	Vec2f textPosition = piePosition + Vec2f(pieSize * 1.3f, pieSize * 1.1f - 0.05f);
	Vec2 textPos = Vec2(textPosition.x, textPosition.y);

	float totalWeight = getTotal();

	renderValueUnit(font, totalWeight, "ms", textPos + Vec2(0.35, 0.035), Vec3f(1,1,1));

	for(int i = 0; i < parts.size(); i++) {
		Vec2 linePos = textPos + Vec2(0, -i*0.035);
		font->render(parts[i].label, linePos, parts[i].color, 0.0006);

		/*std::stringstream percent;
		percent.precision(4);
		percent << parts[i].weight/totalWeight * 100;
		percent << "%";
		font->render(percent.str(), linePos + Vec2(0.2, 0), parts[i].color, 0.0006);*/

		renderValueUnit(font, parts[i].weight / totalWeight * 100, "%", linePos + Vec2(0.2, 0), parts[i].color);

		/*std::stringstream timeTaken;
		timeTaken.precision(4);
		timeTaken << parts[i].weight;
		timeTaken << weightUnit;
		font->render(timeTaken.str(), linePos + Vec2(0.35, 0), parts[i].color, 0.0006);*/

		renderValueUnit(font, parts[i].weight, "ms", linePos + Vec2(0.35, 0), parts[i].color);
	}
}

void PieChart::add(PiePart& p) {
	this->parts.push_back(p);
}

void startPieRendering(Screen& screen) {
	glUseProgram(0);
	Vec2 size = screen.screenSize;
	glPushMatrix();
	glScalef(size.y / size.x, 1.0f, 1.0f);
}

void endPieRendering(Screen& screen) {
	glPopMatrix();
}

float PieChart::getTotal() const {
	float totalWeight = 0;
	for(PiePart p : parts)
		totalWeight += p.weight;
	return totalWeight;
}