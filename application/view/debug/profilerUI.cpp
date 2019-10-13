#include "core.h"

#include "profilerUI.h"

#include <GL/glew.h>

#include "../batch/commandBatch.h"
#include "../path/path.h"
#include "../gui/gui.h"
#include "../screen.h"
#include "../font.h"

#include "../physics/datastructures/boundsTree.h"

#include "../physics/math/mathUtil.h"

#include "../texture.h"
#include "../buffers/frameBuffer.h"

#pragma region PieChart

//! PieChart

#define MAX_ANGLE 0.1f

const Vec3f pieColors[30] {
	Vec3f(0.2f,0.2f,1),
	Vec3f(1,0.5f,0),
	Vec3f(1,1,0),
	Vec3f(1,0,1),
	Vec3f(0,1,0),
	Vec3f(0,1,1),
	Vec3f(1,1,1),
	Vec3f(1,0,0),
	Vec3f(0.5f,0,0),
	Vec3f(0,0.5f,0),
	Vec3f(0,0,0.5f),
	Vec3f(0.5f,0.5f,0),
	Vec3f(0.5f,0,0.5f),
	Vec3f(0,0.5f,0.5f),
	Vec3f(0.5f,0.5f,0.5f),
};

void PieChart::renderPie(Screen& screen) const {
	Vec2f cursorPosition = Vec2f(pieSize, 0);

	float oldAngle = 0.0f;
	float newAngle = 0.0f;
	float totalWeight = getTotal();

	if (totalWeight == 0.0)
		return;

	for (DataPoint dataPoint : parts) {
		float angle = float(PI * 2 * dataPoint.weight / totalWeight);
		int subdivisions = int(angle / MAX_ANGLE + 1);
		newAngle += angle;

		Path::circleSegmentFilled(piePosition, pieSize, oldAngle, newAngle, Vec4f(dataPoint.color, 1.0f), subdivisions);

		oldAngle = newAngle;
	}
}

void PieChart::renderText(Screen& screen, Font* font) const {

	// Title
	Vec2f titlePosition = piePosition + Vec2f(pieSize * 1.3f, pieSize * 1.1f);
	Path::text(font, title, 0.001, Vec2(titlePosition.x, titlePosition.y), Vec4f(1, 1, 1, 1));

	Vec2f textPosition = Vec2(piePosition + Vec2f(pieSize * 1.3f, pieSize * 1.1f - 0.05f));

	float totalWeight = getTotal();

	Path::text(font, totalValue, 0.0006, textPosition + Vec2(0.50, 0.035), Vec4(1, 1, 1, 1));

	for (int i = 0; i < parts.size(); i++) {
		const DataPoint& p = parts[i];
		Vec2 linePosition = textPosition + Vec2(0, -i*0.035);
		Path::text(font, p.label, 0.0006, linePosition, Vec4(p.color, 1));

		std::stringstream percent;
		percent.precision(4);
		percent << p.weight/totalWeight * 100;
		percent << "%";
		Path::text(font, percent.str(), 0.0006, linePosition + Vec2(0.35, 0), Vec4(p.color, 1));
		Path::text(font, p.value, 0.0006, linePosition + Vec2(0.50, 0), Vec4(p.color, 1));
	}
}

void PieChart::add(DataPoint& dataPoint) {
	this->parts.push_back(dataPoint);
}

float PieChart::getTotal() const {
	float totalWeight = 0;
	for (DataPoint p : parts)
		totalWeight += p.weight;
	return totalWeight;
}

#pragma endregion

#pragma region BarChart

//! BartChart

void BarChart::render() {
	float titleHeight = 0.045;
	float marginLeft = this->dimension.x * 0.0;
	float marginBottom = this->dimension.y * 0.045;
	float marginTop = titleHeight + 0.05;
	float max = getMaxWeight();

	Vec2f drawingPosition = position + Vec2f(marginLeft, marginBottom);
	Vec2f drawingSize = this->dimension - Vec2f(marginLeft, marginBottom+marginTop);

	float categoryWidth = drawingSize.x / data.width;
	float barWidth = drawingSize.x / ((data.height+0.5) * data.width);

	for (int cl = 0; cl < data.height; cl++) {
		const BarChartClassInfo& info = classes[cl];

		for (int i = 0; i < data.width; i++) {
			const WeightValue& dataPoint = data[cl][i];

			float height = drawingSize.y * dataPoint.weight / max;
			Vec2f topLeft = drawingPosition + Vec2f(categoryWidth * i + barWidth*cl, height);

			Path::rectFilled(topLeft, Vec2f(barWidth, height), 0.0f, Vec4f(info.color, 1.0f));
		}
	}

	Path::text(GUI::font, title, 0.001, position + Vec2f(0, this->dimension.y - titleHeight), GUI::COLOR::WHITE);

	for (int cl = 0; cl < data.height; cl++) {
		const BarChartClassInfo& info = classes[cl];

		for (int i = 0; i < data.width; i++) {
			const WeightValue& dataPoint = data[cl][i];

			Vec2f bottomLeft = drawingPosition + Vec2f(categoryWidth * i + barWidth * cl, 0);
			float height = drawingSize.y * dataPoint.weight / max;

			Vec2f topTextPosition = bottomLeft + Vec2(0, height+drawingSize.y * 0.02);
			//topTextPosition.x *= GUI::screen->dimension.x / GUI::screen->dimension.y;

			Path::text(GUI::font, dataPoint.value, 0.0005, topTextPosition, Vec4(info.color, 1));
		}
	}

	for (int i = 0; i < data.width; i++) {
		Vec2f botLeft = position + Vec2f(marginLeft, 0) + Vec2f(categoryWidth * i, 0);
		Path::text(GUI::font, labels[i], 0.0005, botLeft, GUI::COLOR::WHITE);
	}

	for (int cl = 0; cl < data.height; cl++)
		Path::text(GUI::font, classes[cl].name, 0.0007, drawingPosition + Vec2f(this->dimension.x - 0.3, drawingSize.y - 0.035 * cl), Vec4(classes[cl].color, 1));
}

float BarChart::getMaxWeight() const {
	float best = 0;
	for (const WeightValue& wv : data) {
		if (wv.weight > best)
			best = wv.weight;
	}
	return best;
}

#pragma endregion

#pragma region Tree

//! Tree

void recursiveRenderTree(const TreeNode& tree, const Vec3f& treeColor, Vec2f origin, float allottedWidth, long long maxCost) {
	if (!tree.isLeafNode()) {
		for (int i = 0; i < tree.nodeCount; i++) {
			Vec2f nextStep = origin + Vec2f(-allottedWidth / 2 + allottedWidth * ((tree.nodeCount != 1) ? (float(i) / (tree.nodeCount-1)) : 0.5f), -0.1f);
			float colorDarkning = pow(1.0f * computeCost(tree[i].bounds) / maxCost, 0.25f);

			//Path::bezierVertical(origin, nextStep, 1.0f, Vec4f(treeColor * colorDarkning, 1.0f), 15);
			Path::line(origin, nextStep, 1.0f, Vec4f(treeColor * colorDarkning, 1.0f));

			recursiveRenderTree(tree[i], treeColor, nextStep, allottedWidth / tree.nodeCount, maxCost);
		}
	}

	if (tree.isGroupHead)
		Path::circleFilled(origin, 0.006f, GUI::COLOR::RED, 8);
}

void renderTreeStructure(Screen& screen, const TreeNode& tree, const Vec3f& treeColor, Vec2f origin, float allottedWidth) {
	long long maxCost = computeCost(tree[0].bounds);

	for (int i = 1; i < tree.nodeCount; i++)
		maxCost = std::max(maxCost, computeCost(tree[1].bounds));

	recursiveRenderTree(tree, treeColor, origin, allottedWidth, maxCost);
}

#pragma endregion

#pragma region SlidingDataChart

//! SlidingDataChart

void SlidingChartDataSetInfo::add(float value) {
	if (data.empty()) {
		deviation = 1.0f;
		mean = value;

		data.push(value);
		
		return;
	}

	data.push(value);

	float variance = deviation * deviation;
	float newVariance = variance;
	float newMean = mean;

	if (data.size() > size) {
		float s = size;
		float s1 = s - 1.0f;

		newMean = mean + (value - data.front()) / s;
		float diffMean = newMean - mean;

		newVariance = variance + diffMean * s / s1 * (2.0f * (data.front() - mean) + s1 * diffMean);

		data.pop();
	} else {
		float s = data.size();

		float s1 = s - 1.0f;
		float s2 = s - 2.0f;

		newMean = (mean * s1 + value) / s;
		newVariance = (s2 * variance + (value - newMean) * (value - mean)) / s1;
	}
	
	mean = newMean;
	deviation = sqrt(newVariance);
}

SlidingChart::SlidingChart(const std::string& title, const Vec2& position, const Vec2& dimension) : title(title), Component(position, dimension) {}

void SlidingChart::add(const SlidingChartDataSetInfo& dataSet) {
	dataSets[dataSet.title] = dataSet;
}

void SlidingChart::add(const std::string& title, float value) {
	return dataSets.at(title).add(value);
}

SlidingChartDataSetInfo SlidingChart::get(const std::string& title) {
	return dataSets.at(title);
}

void SlidingChart::render() {
	float axisOffset = 0.03;
	Vec2f yRange = Vec2(-1, 1);
	Vec2f xRange = Vec2f(-GUI::screen->camera.aspect, GUI::screen->camera.aspect) * 2;
	Path::rectUVRange(GUI::screen->blurFrameBuffer->texture->getID(), position, dimension, xRange, yRange, Vec4f(0.4, 0.4, 0.4, 1));
	Path::line(position + Vec2f(-axisOffset, -dimension.y), position + Vec2f(dimension.x + axisOffset, -dimension.y), 2.0f, GUI::COLOR::WHITE);
	Path::line(position + Vec2f(0, axisOffset), position + Vec2f(0, -dimension.y - axisOffset), 2.0f, GUI::COLOR::WHITE);

	for (auto dataSetIterator : dataSets) {
		SlidingChartDataSetInfo& dataSet = dataSetIterator.second;

		float usedDeviaton = fmax(dataSet.deviation, 0.1*dataSet.mean);
		float stepX = dimension.x / dataSet.size;
		float stepY = dimension.y / usedDeviaton / 6.82f;
		float startY = dataSet.mean - usedDeviaton;

		Vec2f bottomLeft = position - Vec2f(0.0f, dimension.y);

		for (int i = 0; i < dataSet.data.size(); i++) {
			float value = dataSet.data._Get_container()[i];

			Vec2f point = bottomLeft + Vec2f(i * stepX, (value - startY) * stepY);

			Path::lineTo(point);
		}

		Path::stroke(dataSet.color, dataSet.lineSize);

		float lastValue = dataSet.data._Get_container()[dataSet.data.size() - 1];
		float lastY = (lastValue - startY) * stepY;
		Path::text(GUI::font, std::to_string(lastValue), 0.0008f, Vec2f((bottomLeft.x + dimension.x) * 1.01, bottomLeft.y + lastY), dataSet.color, Path::TextPivotVC);
	}

	Vec2f titleSize = GUI::font->size(title, 0.001f);
	Path::text(GUI::font, title, 0.001f, Vec2f(position.x + dimension.x / 2.0, position.y + axisOffset), GUI::COLOR::WHITE, Path::TextPivotHC);
}

Vec2 SlidingChart::resize() {
	return dimension;
}

#pragma endregion