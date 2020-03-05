
#include "../engine/visualData.h"
#include "../graphics/visualShape.h"
#include "../graphics/mesh/indexedMesh.h"
#include "../physics/geometry/shapeClass.h"

namespace Engine::MeshRegistry {
extern std::vector<Graphics::IndexedMesh*> meshes;
extern std::map<const ShapeClass*, VisualData> shapeClassMeshIds;
extern VisualData box;
extern VisualData sphere;
extern VisualData cylinder;

void init();

VisualData addMeshShape(const Graphics::VisualShape& mesh);
VisualData registerMeshFor(const ShapeClass* shapeClass, const Graphics::VisualShape& mesh);
VisualData registerMeshFor(const ShapeClass* shapeClass);
VisualData getOrCreateMeshFor(const ShapeClass* shapeClass);
};
