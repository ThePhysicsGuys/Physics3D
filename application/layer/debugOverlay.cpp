#include "core.h"

#include "debugOverlay.h"

#include "view/screen.h"
#include "shader/shaders.h"

#include "../graphics/renderUtils.h"
#include "../graphics/debug/profilerUI.h"
#include "../graphics/debug/visualDebug.h"
#include "../graphics/path/path.h"
#include "../graphics/gui/gui.h"

#include "../physics/physicsProfiler.h"
#include "../physics/misc/toString.h"
#include "../physics/sharedLockGuard.h"

#include "worlds.h"

namespace Application {

BarChartClassInfo iterChartClasses[] {
	{ "GJK Collide"   , Vec3f(0.2f, 0.2f, 1.0f) },
	{ "GJK No Collide", Vec3f(1.0f, 0.5f, 0.0f) },
	{ "EPA"           , Vec3f(1.0f, 1.0f, 0.0f) }
};

BarChart iterationChart("Iteration Statistics", "", GJKCollidesIterationStatistics.labels, iterChartClasses, Vec2f(-1.0f + 0.1f, -0.3f), Vec2f(0.8f, 0.6f), 3, 17);

SlidingChart fpsSlidingChart("Fps Fps", Vec2f(-0.3, 0.2), Vec2f(0.7, 0.4));

DebugOverlay::DebugOverlay() {

}

DebugOverlay::DebugOverlay(Screen* screen, char flags) : Layer("Debug overlay", screen, flags) {

}

void DebugOverlay::onInit() {
	fpsSlidingChart.add(SlidingChartDataSetInfo("Fps 1", 100, COLOR::ORANGE, 2.0));
	fpsSlidingChart.add(SlidingChartDataSetInfo("Fps 2", 50, COLOR::BLUE, 1.0));
}

void DebugOverlay::onUpdate() {
	using namespace Debug;
	fieldIndex = 0;
}

void DebugOverlay::onEvent(Event& event) {

}

void DebugOverlay::onRender() {

	Renderer::disableDepthTest();
	ApplicationShaders::fontShader.updateProjection(screen->camera.orthoMatrix);

	graphicsMeasure.mark(GraphicsProcess::PROFILER);

	using namespace Debug;
	size_t objectCount = screen->world->getPartCount();
	addDebugField(screen->dimension, GUI::font, "Screen", str(screen->dimension) + ", [" + std::to_string(screen->camera.aspect) + ":1]", "");
	addDebugField(screen->dimension, GUI::font, "Position", str(screen->camera.cframe.position), "");
	addDebugField(screen->dimension, GUI::font, "Objects", objectCount, "");
	//addDebugField(screen->dimension, GUI::font, "Intersections", getTheoreticalNumberOfIntersections(objectCount), "");
	addDebugField(screen->dimension, GUI::font, "AVG Collide GJK Iterations", gjkCollideIterStats.avg(), "");
	addDebugField(screen->dimension, GUI::font, "AVG No Collide GJK Iterations", gjkNoCollideIterStats.avg(), "");
	addDebugField(screen->dimension, GUI::font, "TPS", physicsMeasure.getAvgTPS(), "");
	addDebugField(screen->dimension, GUI::font, "FPS", graphicsMeasure.getAvgTPS(), "");
	/*addDebugField(screen->dimension, GUI::font, "World Kinetic Energy", screen->world->getTotalKineticEnergy(), "");
	addDebugField(screen->dimension, GUI::font, "World Potential Energy", screen->world->getTotalPotentialEnergy(), "");
	addDebugField(screen->dimension, GUI::font, "World Energy", screen->world->getTotalEnergy(), "");*/
	addDebugField(screen->dimension, GUI::font, "World Age", screen->world->age, " ticks");

	if (renderPiesEnabled) {
		Path::bind(GUI::batch);

		float leftSide = float(screen->dimension.x) / float(screen->dimension.y);
		PieChart graphicsPie = toPieChart(graphicsMeasure, "Graphics", Vec2f(-leftSide + 1.5f, -0.7f), 0.2f);
		PieChart physicsPie = toPieChart(physicsMeasure, "Physics", Vec2f(-leftSide + 0.3f, -0.7f), 0.2f);
		PieChart intersectionPie = toPieChart(intersectionStatistics, "Intersections", Vec2f(-leftSide + 2.7f, -0.7f), 0.2f);

		physicsPie.renderText(*screen, GUI::font);
		graphicsPie.renderText(*screen, GUI::font);
		intersectionPie.renderText(*screen, GUI::font);

		physicsPie.renderPie(*screen);
		graphicsPie.renderPie(*screen);
		intersectionPie.renderPie(*screen);

		ParallelArray<long long, 17> gjkColIter = GJKCollidesIterationStatistics.history.avg();
		ParallelArray<long long, 17> gjkNoColIter = GJKNoCollidesIterationStatistics.history.avg();
		ParallelArray<long long, 17> epaIter = EPAIterationStatistics.history.avg();

		for (size_t i = 0; i < GJKCollidesIterationStatistics.size(); i++) {
			iterationChart.data.get(0, i) = WeightValue { (float) gjkColIter[i], std::to_string(gjkColIter[i]) };
			iterationChart.data.get(1, i) = WeightValue { (float) gjkNoColIter[i], std::to_string(gjkNoColIter[i]) };
			iterationChart.data.get(2, i) = WeightValue { (float) epaIter[i], std::to_string(epaIter[i]) };
		}

		iterationChart.position = Vec2f(-leftSide + 0.1f, -0.3);
		iterationChart.render();

		graphicsMeasure.mark(GraphicsProcess::WAIT_FOR_LOCK);
		screen->world->syncReadOnlyOperation([this] () {
			graphicsMeasure.mark(GraphicsProcess::PROFILER);
			renderTreeStructure(*this->screen, this->screen->world->objectTree.rootNode, Vec3f(0, 1, 0), Vec2f(1.4, 0.95), 0.7f);
			renderTreeStructure(*this->screen, this->screen->world->terrainTree.rootNode, Vec3f(0, 0, 1), Vec2f(0.4, 0.95), 0.7f);
			});

		fpsSlidingChart.add("Fps 1", graphicsMeasure.getAvgTPS());
		fpsSlidingChart.add("Fps 2", physicsMeasure.getAvgTPS());
		fpsSlidingChart.render();

		GUI::batch->submit();
	}
}

void DebugOverlay::onClose() {

}

};