
#include "../engine/visualData.h"
#include "../graphics/visualShape.h"
#include "../graphics/mesh/indexedMesh.h"
#include "../physics/geometry/shapeClass.h"

namespace Engine::MeshRegistry {
extern std::vector<::IndexedMesh*> meshes;
extern std::map<const ShapeClass*, VisualData> shapeClassMeshIds;
extern VisualData box;
extern VisualData sphere;
extern VisualData cylinder;

void init();

VisualData addMeshShape(const VisualShape& mesh);
VisualData registerMeshFor(const ShapeClass* shapeClass, const VisualShape& mesh);
VisualData registerMeshFor(const ShapeClass* shapeClass);
VisualData getOrCreateMeshFor(const ShapeClass* shapeClass);
};
