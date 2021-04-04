#include "core.h"

#include "profilerUI.h"

#include "../application/picker/selection.h" // reverse dependency -- BAAAAD
#include "../application/picker/tools/selectionTool.h" // reverse dependency -- BAAAAD
#include "../application/extendedPart.h" // reverse dependency -- BAAAAD

#include <sstream>
#include <GL/glew.h>

#include "texture.h"
#include "path/path.h"
#include "gui/gui.h"
#include "font.h"
#include "../physics/math/constants.h"
#include "../physics/datastructures/boundsTree.h"

namespace P3D::Graphics {

#pragma region PieChart

//! PieChart

#define MAX_ANGLE 0.1f

const Color pieColors[30] {
	Color(0.2f,0.2f,1),
	Color(1,0.5f,0),
	Color(1,1,0),
	Color(1,0,1),
	Color(0,1,0),
	Color(0,1,1),
	Color(1,1,1),
	Color(1,0,0),
	Color(0.5f,0,0),
	Color(0,0.5f,0),
	Color(0,0,0.5f),
	Color(0.5f,0.5f,0),
	Color(0.5f,0,0.5f),
	Color(0,0.5f,0.5f),
	Color(0.5f,0.5f,0.5f),
};

void PieChart::renderPie() const {
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

		Path::circleSegmentFilled(piePosition, pieSize, oldAngle, newAngle, dataPoint.color, subdivisions);

		oldAngle = newAngle;
	}
}

void PieChart::renderText(Graphics::Font* font) const {

	// Title
	Vec2f titlePosition = piePosition + Vec2f(pieSize * 1.3f, pieSize * 1.1f);
	Path::text(font, title, 0.001, Vec2(titlePosition.x, titlePosition.y), Vec4f(1, 1, 1, 1));

	Vec2f textPosition = Vec2(piePosition + Vec2f(pieSize * 1.3f, pieSize * 1.1f - 0.05f));
	float totalWeight = getTotal();

	Path::text(font, totalValue, 0.0006, textPosition + Vec2(0.50, 0.035), Color(1, 1, 1, 1));

	for (int i = 0; i < parts.size(); i++) {
		const DataPoint& p = parts[i];
		Vec2 linePosition = textPosition + Vec2(0, -i * 0.035);
		Path::text(font, p.label, 0.0006, linePosition, p.color);

		std::stringstream percent;
		percent.precision(4);
		percent << p.weight / totalWeight * 100;
		percent << "%";
		Path::text(font, percent.str(), 0.0006, linePosition + Vec2(0.35, 0), p.color);
		Path::text(font, p.value, 0.0006, linePosition + Vec2(0.50, 0), p.color);
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

//! BarChart

void BarChart::render() {
	float titleHeight = 0.045f;
	float marginLeft = 0.0f;
	float marginBottom = this->dimension.y * 0.045f;
	float marginTop = titleHeight + 0.05f;
	float max = getMaxWeight();

	Vec2f drawingPosition = position + Vec2f(marginLeft, marginBottom);
	Vec2f drawingSize = this->dimension - Vec2f(marginLeft, marginBottom + marginTop);

	float categoryWidth = drawingSize.x / data.w;
	float barWidth = drawingSize.x / ((data.h + 0.5f) * data.w);

	for (int cl = 0; cl < data.h; cl++) {
		const BarChartClassInfo& info = classes[cl];

		for (int i = 0; i < data.w; i++) {
			const WeightValue& dataPoint = data(cl, i);

			float height = drawingSize.y * dataPoint.weight / max;
			Vec2f bottomLeft = drawingPosition + Vec2f(categoryWidth * i + barWidth * cl, 0);

			Path::rectFilled(bottomLeft, Vec2f(barWidth, height), 0.0f, info.color);
		}
	}

	Path::text(GUI::font, title, 0.001, position + Vec2f(0, this->dimension.y - titleHeight), Colors::WHITE);

	for (int cl = 0; cl < data.h; cl++) {
		const BarChartClassInfo& info = classes[cl];

		for (int i = 0; i < data.w; i++) {
			const WeightValue& dataPoint = data(cl, i);

			Vec2f bottomLeft = drawingPosition + Vec2f(categoryWidth * i + barWidth * cl, 0);
			float height = drawingSize.y * dataPoint.weight / max;

			Vec2f topTextPosition = bottomLeft + Vec2(0, height + drawingSize.y * 0.02);
			//topTextPosition.x *= GUI::screen->dimension.x / GUI::screen->dimension.y;

			Path::text(GUI::font, dataPoint.value, 0.0005, topTextPosition, info.color);
		}
	}

	for (int i = 0; i < data.w; i++) {
		Vec2f botLeft = position + Vec2f(marginLeft, 0) + Vec2f(categoryWidth * i, 0);
		Path::text(GUI::font, labels[i], 0.0005, botLeft, Colors::WHITE);
	}

	for (int cl = 0; cl < data.h; cl++)
		Path::text(GUI::font, classes[cl].name, 0.0007, drawingPosition + Vec2f(this->dimension.x - 0.3f, drawingSize.y - 0.035f * cl), classes[cl].color);
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

static void recursiveRenderTree(const P3D::OldBoundsTree::TreeNode& tree, const Vec3f& treeColor, Vec2f origin, float allottedWidth, long long maxCost, const void* selectedObject) {
	if (!tree.isLeafNode()) {
		for (int i = 0; i < tree.nodeCount; i++) {
			Vec2f nextStep = origin + Vec2f(-allottedWidth / 2 + allottedWidth * ((tree.nodeCount != 1) ? (float(i) / (tree.nodeCount - 1)) : 0.5f), -0.1f);
			float colorDarkning = pow(1.0f * P3D::OldBoundsTree::computeCost(tree[i].bounds) / maxCost, 0.25f);

			//Path::bezierVertical(origin, nextStep, 1.0f, Vec4f(treeColor * colorDarkning, 1.0f), 15);
			Path::line(origin, nextStep, join(treeColor * colorDarkning, 1.0f), 0.5f);

			recursiveRenderTree(tree[i], treeColor, nextStep, allottedWidth / tree.nodeCount, maxCost, selectedObject);
		}
	}

	if (tree.object == selectedObject) {
		Path::circleFilled(origin, 0.012f, Colors::YELLOW, 8);
	}

	if (tree.isGroupHead) {
		Path::circleFilled(origin, 0.006f, Colors::RED, 8);
	}
}

void renderTreeStructure(const P3D::OldBoundsTree::BoundsTree<Part>& tree, const Vec3f& treeColor, Vec2f origin, float allottedWidth, const void* selectedObject) {
	if (tree.isEmpty()) {
		return;
	}
	long long maxCost = P3D::OldBoundsTree::computeCost(tree.rootNode.bounds);

	recursiveRenderTree(tree.rootNode, treeColor, origin, allottedWidth, maxCost, selectedObject);
}

static void recursiveRenderTree(const P3D::NewBoundsTree::TreeTrunk& curTrunk, int curTrunkSize, const Vec3f& treeColor, Vec2f origin, float allottedWidth, float maxCost, const void* selectedObject) {
	for(int i = 0; i < curTrunkSize; i++) {
		const P3D::NewBoundsTree::TreeNodeRef& subNode = curTrunk.subNodes[i];

		Vec2f nextStep = origin + Vec2f(-allottedWidth / 2 + allottedWidth * ((curTrunkSize != 1) ? (float(i) / (curTrunkSize - 1)) : 0.5f), -0.1f);
		float colorDarkning = pow(1.0f * P3D::NewBoundsTree::computeCost(curTrunk.getBoundsOfSubNode(i)) / maxCost, 0.25f);

		//Path::bezierVertical(origin, nextStep, 1.0f, Vec4f(treeColor * colorDarkning, 1.0f), 15);
		Path::line(origin, nextStep, join(treeColor * colorDarkning, 1.0f), 0.5f);

		if(subNode.isTrunkNode()) {
			recursiveRenderTree(subNode.asTrunk(), subNode.getTrunkSize(), treeColor, nextStep, allottedWidth / curTrunkSize, maxCost, selectedObject);

			if(subNode.isGroupHead()) {
				Path::circleFilled(nextStep, 0.006f, Colors::RED, 8);
			}
		} else {
			P3D::Application::ExtendedPart* extPart = reinterpret_cast<Application::ExtendedPart*>(subNode.asObject());
			if(P3D::Application::SelectionTool::selection.contains(extPart->entity)) {
				Path::circleFilled(nextStep, 0.012f, Colors::YELLOW, 8);
			}
		}
	}
}

void renderTreeStructure(const P3D::NewBoundsTree::BoundsTree<Part>& tree, const Vec3f& treeColor, Vec2f origin, float allottedWidth, const void* selectedObject) {
	if(tree.isEmpty()) {
		return;
	}
	auto base = tree.getPrototype().getBaseTrunk();
	float maxCost = P3D::NewBoundsTree::computeCost(base.first.getBoundsOfSubNode(0));
	for(int i = 1; i < base.second; i++) {
		float subCost = P3D::NewBoundsTree::computeCost(base.first.getBoundsOfSubNode(i));
		if(subCost > maxCost) maxCost = subCost;
	}

	recursiveRenderTree(base.first, base.second, treeColor, origin, allottedWidth, maxCost, selectedObject);
}

#pragma endregion

#pragma region SlidingDataChart

//! SlidingDataChart

void SlidingChartDataSetInfo::add(float value) {
	if (data.size() == 0) {
		deviation = 1.0f;
		mean = value;

		data.add(value);

		return;
	}

	data.add(value);

	float variance = deviation * deviation;
	float newVariance = variance;
	float newMean = mean;

	if (data.size() == size) {
		float s = size;
		float s1 = s - 1.0f;

		newMean = mean + (value - data.front()) / s;
		float diffMean = newMean - mean;

		newVariance = variance + diffMean * s / s1 * (2.0f * (data.front() - mean) + s1 * diffMean);
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

SlidingChart::SlidingChart(const std::string& title, const Vec2& position, const Vec2& dimension) : title(title), position(position), dimension(dimension) {}

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

	Path::rect(position, dimension, 0.0f, Vec4f(0.4f, 0.4f, 0.4f, 1.0f));
	Path::line(position + Vec2f(-axisOffset, -dimension.y), position + Vec2f(dimension.x + axisOffset, -dimension.y), Colors::WHITE, 2.0f);
	Path::line(position + Vec2f(0, axisOffset), position + Vec2f(0, -dimension.y - axisOffset), Colors::WHITE, 2.0f);

	for (auto dataSetIterator : dataSets) {
		SlidingChartDataSetInfo& dataSet = dataSetIterator.second;

		float usedDeviaton = fmax(dataSet.deviation, 0.1 * dataSet.mean);
		float stepX = dimension.x / dataSet.size;
		float stepY = dimension.y / usedDeviaton / 6.82f;
		float startY = dataSet.mean - usedDeviaton;

		Vec2f bottomLeft = position - Vec2f(0.0f, dimension.y);

		int i = 0;
		for (float value : dataSet.data) {
			Vec2f point = bottomLeft + Vec2f(i * stepX, (value - startY) * stepY);

			Path::lineTo(point);

			i++;
		}

		Path::stroke(dataSet.color, dataSet.lineSize);

		float lastValue = dataSet.data.front();
		float lastY = (lastValue - startY) * stepY;
		Path::text(GUI::font, std::to_string(lastValue), 0.0008f, Vec2f((bottomLeft.x + dimension.x) * 1.01f, bottomLeft.y + lastY), dataSet.color, Path::TextPivotVC);
	}

	Vec2f titleSize = GUI::font->size(title, 0.001f);
	Path::text(GUI::font, title, 0.001f, Vec2f(position.x + dimension.x / 2.0f, position.y + axisOffset), Colors::WHITE, Path::TextPivotHC);
}

Vec2 SlidingChart::resize() {
	return Vec2(dimension);
}

#pragma endregion

};
