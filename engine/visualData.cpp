#include "core.h"
#include "visualData.h"

#include "meshRegistry.h"

void VisualData::render(int renderMode) const {
	Engine::MeshRegistry::meshes[this->drawMeshId]->render(renderMode);
}
