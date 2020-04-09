
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

VisualData addMeshShape(Graphics::VisualShape& mesh);
VisualData registerMeshFor(const ShapeClass* shapeClass, Graphics::VisualShape& mesh);
VisualData registerMeshFor(const ShapeClass* shapeClass);
VisualData getOrCreateMeshFor(const ShapeClass* shapeClass);

Graphics::VisualShape createCube(float size);
Graphics::VisualShape createBox(float width, float height, float depth);
Graphics::VisualShape createSphere(double radius, int steps = 1);
Graphics::VisualShape createCylinder(int sides, double radius, double height);

};
