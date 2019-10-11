#pragma once

#include <sstream>
#include <queue>

#include "../engine/profiling.h"
#include "../engine/math/linalg/largeMatrix.h"
#include "../gui/component.h"

class Font;
class Screen;
struct TreeNode;

extern const Vec3f pieColors[30];

struct WeightValue {
	float weight;
	std::string value;
};

struct DataPoint {
	float weight;
	std::string value;
	Vec3f color;
	const char* label;
	DataPoint() : color(), weight(0) {}
	DataPoint(float weight, std::string value, Vec3f color, const char* label) : weight(weight), value(value), color(color), label(label) {}
};

struct PieChart {
	const char* title;
	Vec2f piePosition;
	float pieSize;
	std::vector<DataPoint> parts;
	std::string totalValue;

	inline PieChart(const char* title, std::string totalValue, Vec2f piePosition, float pieSize) : title(title), totalValue(totalValue), piePosition(piePosition), pieSize(pieSize) {}
	void renderPie(Screen& screen) const;
	void renderText(Screen& screen, Font* font) const;
	void add(DataPoint& p);
	float getTotal() const;
};

struct BarChartClassInformation {
	std::string name;
	Vec3f color;
};

struct BarChart : public Component {
	const char* title;
	const char** labels;
	BarChartClassInformation* classes;
	LargeMatrix<WeightValue> data;
	std::string totalValue;

	inline BarChart(const char* title, std::string totalValue, const char** labels, BarChartClassInformation* classes, Vec2f chartPosition, Vec2f chartSize, int classCount, int barCount) : 
		title(title), totalValue(totalValue), classes(classes), labels(labels), data(barCount, classCount), Component(chartPosition, chartSize) {}
	void render() override;
	inline Vec2 resize() override { return dimension; };

	float getMaxWeight() const;
};

struct SlidingDataChart : public Component {
	const char* title;

	int size;
	std::queue<float> data;

	float mean;
	float deviation;

	inline SlidingDataChart(const char* title, int size, Vec2 position, Vec2 dimension) : title(title), size(size), mean(0), deviation(1), Component(position, dimension) {};

	void add(float value);
	void render() override;
	inline Vec2 resize() override { return dimension;  }
};

void renderTreeStructure(Screen& screen, const TreeNode& tree, const Vec3f& treeColor, Vec2f origin, float allottedWidth);

