#include "core.h"
#include "debugOverlay.h"

#include "view/screen.h"
#include "shader/shaders.h"

#include "../util/resource/resourceManager.h"

#include "../graphics/renderer.h"
#include "../graphics/debug/profilerUI.h"
#include "../graphics/debug/visualDebug.h"
#include "../graphics/resource/fontResource.h"
#include "../graphics/path/path.h"
#include "../graphics/gui/gui.h"

#include <Physics3D/misc/physicsProfiler.h>
#include <Physics3D/misc/toString.h>
#include <Physics3D/world.h>
#include <Physics3D/layer.h>

#include "worlds.h"

namespace P3D::Application {

Graphics::BarChartClassInfo iterChartClasses[] {
	{ "GJK Collide"   , Vec3f(0.2f, 0.2f, 1.0f) },
	{ "GJK No Collide", Vec3f(1.0f, 0.5f, 0.0f) },
	{ "EPA"           , Vec3f(1.0f, 1.0f, 0.0f) }
};

Graphics::BarChart iterationChart("Iteration Statistics", "", GJKCollidesIterationStatistics.labels, iterChartClasses, Vec2f(-1.0f + 0.1f, -0.3f), Vec2f(0.8f, 0.6f), 3, 17);
Graphics::SlidingChart fpsSlidingChart("Fps Fps", Vec2f(-0.3f, 0.2f), Vec2f(0.7f, 0.4f));

void DebugOverlay::onInit(Engine::Registry64& registry) {
	using namespace Graphics;
	fpsSlidingChart.add(SlidingChartDataSetInfo("Fps 1", 100, Colors::ORANGE, 2.0));
	fpsSlidingChart.add(SlidingChartDataSetInfo("Fps 2", 50, Colors::BLUE, 1.0));
}

void DebugOverlay::onUpdate(Engine::Registry64& registry) {
	using namespace Graphics::VisualDebug;
	fieldIndex = 0;
}

void DebugOverlay::onEvent(Engine::Registry64& registry, Engine::Event& event) {

}

void DebugOverlay::onRender(Engine::Registry64& registry) {
	using namespace Graphics;
	using namespace VisualDebug;
	using namespace Renderer;

	Screen* screen = static_cast<Screen*>(this->ptr);

	Graphics::Shaders::guiShader->bind();
	Graphics::Shaders::guiShader->setUniform("projectionMatrix", screen->camera.orthoMatrix);

	beginScene();

	enableBlending();
	disableDepthTest();
	disableCulling();

	Path::bind(GUI::batch);
	ResourceManager::get<FontResource>("font")->getAtlas()->bind();
	Shaders::fontShader->updateProjection(screen->camera.orthoMatrix);
	Graphics::Shaders::guiShader->setUniform("projectionMatrix", screen->camera.orthoMatrix);
	graphicsMeasure.mark(GraphicsProcess::PROFILER);

	size_t objectCount = screen->world->getPartCount();
	addDebugField(screen->dimension, GUI::font, "Screen", str(screen->dimension) + ", [" + std::to_string(screen->camera.aspect) + ":1]", "");
	addDebugField(screen->dimension, GUI::font, "Position", str(screen->camera.cframe.position), "");
	addDebugField(screen->dimension, GUI::font, "Objects", objectCount, "");
	//addDebugField(screen->dimension, GUI::font, "Intersections", getTheoreticalNumberOfIntersections(objectCount), "");
	addDebugField(screen->dimension, GUI::font, "AVG Collide GJK Iterations", gjkCollideIterStats.avg(), "");
	addDebugField(screen->dimension, GUI::font, "AVG No Collide GJK Iterations", gjkNoCollideIterStats.avg(), "");
	addDebugField(screen->dimension, GUI::font, "TPS", physicsMeasure.getAvgTPS(), "");
	addDebugField(screen->dimension, GUI::font, "FPS", Graphics::graphicsMeasure.getAvgTPS(), "");
	/*addDebugField(screen->dimension, GUI::font, "World Kinetic Energy", screen->world->getTotalKineticEnergy(), "");
	addDebugField(screen->dimension, GUI::font, "World Potential Energy", screen->world->getTotalPotentialEnergy(), "");
	addDebugField(screen->dimension, GUI::font, "World Energy", screen->world->getTotalEnergy(), "");*/
	addDebugField(screen->dimension, GUI::font, "World Age", screen->world->age, " ticks");

	if (renderPiesEnabled) {
		float leftSide = float(screen->dimension.x) / float(screen->dimension.y);
		PieChart graphicsPie = toPieChart(Graphics::graphicsMeasure, "Graphics", Vec2f(-leftSide + 1.5f, -0.7f), 0.2f);
		PieChart physicsPie = toPieChart(physicsMeasure, "Physics", Vec2f(-leftSide + 0.3f, -0.7f), 0.2f);
		PieChart intersectionPie = toPieChart(intersectionStatistics, "Intersections", Vec2f(-leftSide + 2.7f, -0.7f), 0.2f);

		physicsPie.renderText(GUI::font);
		graphicsPie.renderText(GUI::font);
		intersectionPie.renderText(GUI::font);

		physicsPie.renderPie();
		graphicsPie.renderPie();
		intersectionPie.renderPie();

		ParallelArray<long long, 17> gjkColIter = GJKCollidesIterationStatistics.history.avg();
		ParallelArray<long long, 17> gjkNoColIter = GJKNoCollidesIterationStatistics.history.avg();
		ParallelArray<long long, 17> epaIter = EPAIterationStatistics.history.avg();

		for (size_t i = 0; i < GJKCollidesIterationStatistics.size(); i++) {
			iterationChart.data(0, i) = WeightValue { (float) gjkColIter[i], std::to_string(gjkColIter[i]) };
			iterationChart.data(1, i) = WeightValue { (float) gjkNoColIter[i], std::to_string(gjkNoColIter[i]) };
			iterationChart.data(2, i) = WeightValue { (float) epaIter[i], std::to_string(epaIter[i]) };
		}

		iterationChart.position = Vec2f(-leftSide + 0.1f, -0.3f);
		iterationChart.render();

		graphicsMeasure.mark(GraphicsProcess::WAIT_FOR_LOCK);
		screen->world->syncReadOnlyOperation([this]() {
			Screen* screen = static_cast<Screen*>(this->ptr);

			graphicsMeasure.mark(GraphicsProcess::PROFILER);
			
			size_t layerCount = getMaxLayerID(screen->world->layers);
			Vec2i d = screen->dimension;
			float availableSpace = float(d.x) / float(d.y);
			float widthPerTree = availableSpace / layerCount;
			int i = 0;
			for(const ColissionLayer& clayer : screen->world->layers) {
				for(const WorldLayer& layer : clayer.subLayers) {
					float xPos = widthPerTree / 2 + widthPerTree * i;

					renderTreeStructure(layer.tree, pieColors[i], Vec2f(xPos, 0.95f), widthPerTree * 0.7f);
					i++;
				}
			}
		});

		/*fpsSlidingChart.add("Fps 1", Graphics::graphicsMeasure.getAvgTPS());
		fpsSlidingChart.add("Fps 2", physicsMeasure.getAvgTPS());
		fpsSlidingChart.render();*/

		Path::batch->pushCommand(0); // if this is not here then the trees won't render properly if fpsSlidingChart.render is commented for some reason
	}
	
	GUI::batch->submit();

	endScene();
}

void DebugOverlay::onClose(Engine::Registry64& registry) {

}

};