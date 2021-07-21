#pragma once

#include <vector>
#include "../bindable.h"
#include "../buffers/bufferLayout.h"
#include "../meshRegistry.h"
#include "../util/log.h"
#include "../graphics/shader/shader.h"

namespace P3D::Graphics {

template<typename Uniform>
class OrderedBatchManager {
private:
	std::vector<std::pair<GLID, Uniform>> batches;

public:
	OrderedBatchManager() {}

	template<typename... Args>
	void add(GLID mesh, Args&&... uniform) {
		if (mesh >= MeshRegistry::meshes.size()) {
			Log::error("Trying to add a mesh that has not been registered in the mesh registry");
			return;
		}

		batches.push_back(std::make_pair(mesh, Uniform { std::forward<Args>(uniform)... }));
	}

	void submit(Shader* shader) {
		shader->bind();
		for (const auto& [mesh, uniform] : batches) {
			shader->setUniform("modelMatrix", uniform.modelMatrix);
			shader->setUniform("albedo", uniform.albedo);
			shader->setUniform("roughness", uniform.roughness);
			shader->setUniform("metalness", uniform.metalness);
			shader->setUniform("ambientOcclusion", uniform.ao);
			MeshRegistry::meshes[mesh]->render();
		}
	}

	void clear() {
		batches.clear();
	}
};

using DefaultOrderedBatchManager = OrderedBatchManager<DefaultUniform>;

};