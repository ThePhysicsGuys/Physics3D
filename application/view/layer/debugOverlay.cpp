#include "debugOverlay.h"

#include "../screen.h"

#include "../debug/profilerUI.h"
#include "../debug/visualDebug.h"

#include "../engine/physicsProfiler.h"
#include "../engine/math/mathUtil.h"

#include "../../worlds.h"
#include "../../../engine/sharedLockGuard.h"

// Pies
BarChartClassInformation iterChartClasses[] {{ "GJK Collide", Vec3f(0.2f, 0.2f, 1.0f) }, { "GJK No Collide", Vec3f(1.0f, 0.5f, 0.0f) }, { "EPA", Vec3f(1.0f, 1.0f, 0.0f) }};
BarChart iterationChart("Iteration Statistics", "", GJKCollidesIterationStatistics.labels, iterChartClasses, Vec2f(-1.0f + 0.1f, -0.3f), Vec2f(0.8f, 0.6f), 3, 17);


DebugOverlay::DebugOverlay() {

}

DebugOverlay::DebugOverlay(Screen* screen) : Layer("Debug overlay", screen) {

}

void DebugOverlay::init() {

}

void DebugOverlay::update() {
	fieldIndex = 0;
}

void DebugOverlay::render() {
	graphicsMeasure.mark(GraphicsProcess::PROFILER);

	size_t objCount = screen->world->getPartCount();
	renderDebugField(screen->dimension, GUI::font, "Screen", str(screen->dimension) + ", [" + std::to_string(screen->camera.aspect) + ":1]", "");
	renderDebugField(screen->dimension, GUI::font, "Position", str(screen->camera.cframe.position), "");
	renderDebugField(screen->dimension, GUI::font, "Objects", objCount, "");
	renderDebugField(screen->dimension, GUI::font, "Intersections", getTheoreticalNumberOfIntersections(objCount), "");
	renderDebugField(screen->dimension, GUI::font, "AVG Collide GJK Iterations", gjkCollideIterStats.avg(), "");
	renderDebugField(screen->dimension, GUI::font, "AVG No Collide GJK Iterations", gjkNoCollideIterStats.avg(), "");
	renderDebugField(screen->dimension, GUI::font, "TPS", physicsMeasure.getAvgTPS(), "");
	renderDebugField(screen->dimension, GUI::font, "FPS", graphicsMeasure.getAvgTPS(), "");
	renderDebugField(screen->dimension, GUI::font, "World Kinetic Energy", screen->world->getTotalKineticEnergy(), "");
	renderDebugField(screen->dimension, GUI::font, "World Potential Energy", screen->world->getTotalPotentialEnergy(), "");
	renderDebugField(screen->dimension, GUI::font, "World Energy", screen->world->getTotalEnergy(), "");
	renderDebugField(screen->dimension, GUI::font, "World Age", screen->world->age, " ticks");

	if (renderPiesEnabled) {
		float leftSide = float(screen->dimension.x) / float(screen->dimension.y);
		PieChart graphicsPie = toPieChart(graphicsMeasure, "Graphics", Vec2f(-leftSide + 1.5f, -0.7f), 0.2f);
		PieChart physicsPie = toPieChart(physicsMeasure, "Physics", Vec2f(-leftSide + 0.3f, -0.7f), 0.2f);
		PieChart intersectionPie = toPieChart(intersectionStatistics, "Intersection Statistics", Vec2f(-leftSide + 2.7f, -0.7f), 0.2f);
		physicsPie.renderText(*screen, GUI::font);
		graphicsPie.renderText(*screen, GUI::font);
		intersectionPie.renderText(*screen, GUI::font);
		startPieRendering(*screen);
		physicsPie.renderPie(*screen);
		graphicsPie.renderPie(*screen);
		intersectionPie.renderPie(*screen);
		endPieRendering(*screen);

		ParallelArray<long long, 17> gjkColIter = GJKCollidesIterationStatistics.history.avg();
		ParallelArray<long long, 17> gjkNoColIter = GJKNoCollidesIterationStatistics.history.avg();
		ParallelArray<long long, 17> epaIter = EPAIterationStatistics.history.avg();

		for (size_t i = 0; i < GJKCollidesIterationStatistics.size(); i++) {
			iterationChart.data[0][i] = WeightValue{ (float)gjkColIter[i], std::to_string(gjkColIter[i]) };
			iterationChart.data[1][i] = WeightValue{ (float)gjkNoColIter[i], std::to_string(gjkNoColIter[i]) };
			iterationChart.data[2][i] = WeightValue{ (float)epaIter[i], std::to_string(epaIter[i]) };
		}

		iterationChart.position = Vec2f(-leftSide + 0.1f, -0.3);
		iterationChart.render();

		SharedLockGuard lg(screen->world->lock);
		renderTreeStructure(*screen, screen->world->objectTree.rootNode, Vec3f(0, 1, 0), Vec2f(screen->camera.aspect, 0.95), 2.0);
	}
}

void DebugOverlay::close() {}
