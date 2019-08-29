#include "profilerUI.h"

#include <GL/glew.h>

#include <algorithm>

#include "../engine/math/vec.h"
#include "../engine/math/mat2.h"

#include "../batch/commandBatch.h"
#include "../gui/path.h"
#include "../gui/gui.h"
#include "../screen.h"
#include "../font.h"

#include "../engine/datastructures/boundsTree.h"

#define _USE_MATH_DEFINES
#include "math.h"

#define MAX_ANGLE 0.1f

const Vec3f pieColors[30]{
	Vec3f(1,0,0),
	Vec3f(0,1,0),
	Vec3f(0.2f,0.2f,1),
	Vec3f(1,0.5f,0),
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
	for(DataPoint p : parts) {
		float angle = float(M_PI * 2 * p.weight / totalWeight);

		int subdivisions = int(angle / MAX_ANGLE + 1);
		
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

/*void PieChart::renderValueUnit(Font* font, float value, const char* unit, Vec2 position, Vec3f color) const {
	std::stringstream ss;
	ss.precision(4);
	ss << value;
	ss << unit;
	font->render(ss.str(), position, color, 0.0006);
}*/

void PieChart::renderText(Screen& screen, Font* font) const {
	Path::bind(GUI::batch);

	// Title
	Vec2f titlePosition = piePosition + Vec2f(pieSize * 1.3f, pieSize * 1.1f);
	Path::text(font, title, Vec2(titlePosition.x, titlePosition.y), Vec4f(1, 1, 1, 1), 0.001);

	Vec2f textPosition = piePosition + Vec2f(pieSize * 1.3f, pieSize * 1.1f - 0.05f);
	Vec2 textPos = Vec2(textPosition.x, textPosition.y);

	float totalWeight = getTotal();

	Path::text(font, totalValue, textPos + Vec2(0.50, 0.035), Vec4(1,1,1,1), 0.0006);

	for(int i = 0; i < parts.size(); i++) {
		const DataPoint& p = parts[i];
		Vec2 linePos = textPos + Vec2(0, -i*0.035);
		Path::text(font, p.label, linePos, Vec4(p.color, 1), 0.0006);

		std::stringstream percent;
		percent.precision(4);
		percent << p.weight/totalWeight * 100;
		percent << "%";
		Path::text(font, percent.str(), linePos + Vec2(0.35, 0), Vec4(p.color, 1), 0.0006);
		Path::text(font, p.value, linePos + Vec2(0.50, 0), Vec4(p.color, 1), 0.0006);
	}

	GUI::batch->submit();
}

void PieChart::add(DataPoint& p) {
	this->parts.push_back(p);
}

void startPieRendering(Screen& screen) {
	glUseProgram(0);
	Vec2i size = screen.dimension;
	glPushMatrix();
	glScalef(float(size.y) / size.x, 1.0f, 1.0f);
}

void endPieRendering(Screen& screen) {
	glPopMatrix();
}

float PieChart::getTotal() const {
	float totalWeight = 0;
	for(DataPoint p : parts)
		totalWeight += p.weight;
	return totalWeight;
}

void BarChart::render() {
	Path::bind(GUI::batch);

	glUseProgram(0);

	float titleHeight = 0.045;

	float marginLeft = this->dimension.x * 0.0;
	float marginBottom = this->dimension.y * 0.045;
	float marginTop = titleHeight + 0.05;

	Vec2f drawingPosition = position + Vec2f(marginLeft, marginBottom);

	Vec2f drawingSize = this->dimension - Vec2f(marginLeft, marginBottom+marginTop);

	float categoryWidth = drawingSize.x / data.width;
	float barWidth = drawingSize.x / ((data.height+0.5) * data.width);

	float max = getMaxWeight();

	glPushMatrix();
	glScaled(float(GUI::screen->dimension.y) / GUI::screen->dimension.x, 1, 1);

	//Log::debug("Screenres: x=%d, y=%d", GUI::screen->dimension.x, GUI::screen->dimension.y);

	glBegin(GL_QUADS);
	/*color(Vec3f(0.7, 0.7, 0.7));
	vertex(position);
	vertex(position + Vec2f(size.x, 0));
	vertex(position + size);
	vertex(position + Vec2f(0, size.y));*/
	for (int cl = 0; cl < data.height; cl++) {
		const BarChartClassInformation& info = classes[cl];
		color(info.color);
		for (int i = 0; i < data.width; i++) {
			const WeightValue& dataPoint = data[cl][i];

			Vec2f botLeft = drawingPosition + Vec2f(categoryWidth * i + barWidth*cl, 0);

			float height = drawingSize.y * dataPoint.weight / max;

			vertex(botLeft);
			vertex(botLeft + Vec2f(barWidth, 0));
			vertex(botLeft + Vec2f(barWidth, height));
			vertex(botLeft + Vec2f(0, height));
		}
	}
	glEnd();
	glPopMatrix();

	Path::text(GUI::font, title, position + Vec2f(0, this->dimension.y - titleHeight), GUI::COLOR::WHITE, 0.001);

	for (int cl = 0; cl < data.height; cl++) {
		const BarChartClassInformation& info = classes[cl];
		color(info.color);
		for (int i = 0; i < data.width; i++) {
			const WeightValue& dataPoint = data[cl][i];

			Vec2f botLeft = drawingPosition + Vec2f(categoryWidth * i + barWidth * cl, 0);

			float height = drawingSize.y * dataPoint.weight / max;

			Vec2f topTextPosition = botLeft + Vec2(0, height+drawingSize.y * 0.02);
			//topTextPosition.x *= GUI::screen->dimension.x / GUI::screen->dimension.y;

			Path::text(GUI::font, dataPoint.value, topTextPosition, Vec4(info.color, 1), 0.0005);
		}
	}

	for (int i = 0; i < data.width; i++) {
		Vec2f botLeft = position + Vec2f(marginLeft, 0) + Vec2f(categoryWidth * i, 0);
		//botLeft.x *= GUI::screen->dimension.x / GUI::screen->dimension.y;
		Path::text(GUI::font, labels[i], botLeft, GUI::COLOR::WHITE, 0.0005);
	}

	for (int cl = 0; cl < data.height; cl++) {
		Path::text(GUI::font, classes[cl].name, drawingPosition + Vec2f(this->dimension.x - 0.3, drawingSize.y - 0.035 * cl), Vec4(classes[cl].color, 1), 0.0007);
	}

	GUI::batch->submit();
}

float BarChart::getMaxWeight() const {
	float best = 0;
	for (const WeightValue& wv:data){
		float w = wv.weight;
		if (w > best) {
			best = w;
		}
	}
	return best;
}

void recursiveRenderTree(const TreeNode& tree, const Vec3f& treeColor, Vec2f origin, float allottedWidth, long long maxCost) {
	if (!tree.isLeafNode()) {
		for (int i = 0; i < tree.nodeCount; i++) {
			Vec2f nextStep = origin + Vec2f(-allottedWidth / 2 + allottedWidth * ((tree.nodeCount != 1)?(float(i) / (tree.nodeCount-1)):0.5f), -0.04f);
			float colorDarkning = pow(1.0f * computeCost(tree[i].bounds) / maxCost, 0.25f);

			/*color(treeColor * colorDarkning);
			vertex(origin);
			vertex(nextStep);*/

			Path::line(origin, nextStep, 1.0f, Vec4f(treeColor * colorDarkning, 1.0f), Vec4f(treeColor * colorDarkning, 1.0f));
		

			recursiveRenderTree(tree[i], treeColor, nextStep, allottedWidth / tree.nodeCount, maxCost);
		}
	}
	if (!tree.divisible) Path::circleFilled(origin, 0.006, GUI::COLOR::RED, 8);
}

void renderTreeStructure(Screen& screen, const TreeNode& tree, const Vec3f& treeColor, Vec2f origin, float allottedWidth) {
	//glUseProgram(0);

	Vec2i size = screen.dimension;

	long long maxCost = computeCost(tree[0].bounds);
	for (int i = 1; i < tree.nodeCount; i++) {
		maxCost = std::max(maxCost, computeCost(tree[1].bounds));
	}

	/*glPushMatrix();
	glScalef(float(size.y) / size.x, 1.0f, 1.0f);

	

	//
	glScaled(float(GUI::screen->dimension.y) / GUI::screen->dimension.x, 1, 1);
	glBegin(GL_LINES);*/
	recursiveRenderTree(tree, treeColor, origin, allottedWidth, maxCost);
	/*glEnd();
	glPopMatrix();*/

	GUI::batch->submit();
}
