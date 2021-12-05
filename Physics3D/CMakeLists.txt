cmake_minimum_required(VERSION 3.10)

project(Physics3D VERSION 0.9)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")

	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Oi /ot /GL")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

	#running benchmarks showed this to be a pessimization
	#set(CMAKE_INTERPROCEDURAL_OPTIMIZATION True)
	#surprisingly, also a pessimization
	#set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -ffast-math")
endif()


add_library(Physics3D STATIC 
  part.cpp
  physical.cpp
  rigidBody.cpp
  layer.cpp
  world.cpp
  worldPhysics.cpp
  inertia.cpp

  math/linalg/eigen.cpp
  math/linalg/trigonometry.cpp

  geometry/computationBuffer.cpp
  geometry/convexShapeBuilder.cpp
  geometry/genericIntersection.cpp
  geometry/indexedShape.cpp
  geometry/intersection.cpp
  geometry/triangleMesh.cpp
  geometry/triangleMeshSSE.cpp
  geometry/triangleMeshAVX.cpp
  geometry/polyhedron.cpp
  geometry/shape.cpp
  geometry/shapeBuilder.cpp
  geometry/shapeClass.cpp
  geometry/shapeCreation.cpp
  geometry/builtinShapeClasses.cpp
  geometry/shapeLibrary.cpp

  datastructures/aligned_alloc.cpp

  boundstree/boundsTree.cpp
  boundstree/filters/visibilityFilter.cpp
  
  hardconstraints/fixedConstraint.cpp
  hardconstraints/hardConstraint.cpp
  hardconstraints/hardPhysicalConnection.cpp
  hardconstraints/motorConstraint.cpp
  hardconstraints/controller/sineWaveController.cpp
  
  constraints/constraint.cpp
  constraints/constraintGroup.cpp
  constraints/ballConstraint.cpp
  constraints/hingeConstraint.cpp
  constraints/barConstraint.cpp
  
  softlinks/softLink.cpp
  softlinks/springLink.cpp
  softlinks/elasticLink.cpp
  softlinks/magneticLink.cpp
  softlinks/alignmentLink.cpp
  
  externalforces/directionalGravity.cpp
  externalforces/externalForce.cpp
  externalforces/magnetForce.cpp
  
  threading/upgradeable_shared_mutex.cpp
  threading/synchronizedWorld.cpp
  threading/physicsThread.cpp
  
  misc/debug.cpp
  misc/cpuid.cpp
  misc/validityHelper.cpp
  misc/physicsProfiler.cpp
  
  misc/serialization/serialization.cpp
  misc/serialization/serializeBasicTypes.cpp
)
