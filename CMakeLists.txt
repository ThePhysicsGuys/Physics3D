cmake_minimum_required(VERSION 3.10)

project(Physics3D VERSION 1.9)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#running benchmarks showed this to be a pessimization
#set(CMAKE_INTERPROCEDURAL_OPTIMIZATION True)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -mavx2")
#surprisingly, also a pessimization
#set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -ffast-math")

add_library(
util STATIC 
  util/terminalColor.cpp
  util/log.cpp
  util/properties.cpp
  util/serializeBasicTypes.cpp
  util/stringUtil.cpp
)

add_library(
physics STATIC 
  physics/constraintGroup.cpp
  physics/debug.cpp
  physics/part.cpp
  physics/physical.cpp
  physics/physicsProfiler.cpp
  physics/rigidBody.cpp
  physics/world.cpp
  physics/worldPhysics.cpp

  physics/math/cframe.cpp
  physics/math/fix.cpp

  physics/math/linalg/eigen.cpp
  physics/math/linalg/largeMatrix.cpp
  physics/math/linalg/trigonometry.cpp

  physics/geometry/computationBuffer.cpp
  physics/geometry/convexShapeBuilder.cpp
  physics/geometry/genericIntersection.cpp
  physics/geometry/indexedShape.cpp
  physics/geometry/intersection.cpp
  physics/geometry/polyhedron.cpp
  physics/geometry/polyhedronInternals.cpp
  physics/geometry/shape.cpp
  physics/geometry/shapeBuilder.cpp
  physics/geometry/shapeClass.cpp

  physics/datastructures/alignedPtr.cpp
  physics/datastructures/boundsTree.cpp

  physics/constraints/fixedConstraint.cpp
  physics/constraints/hardConstraint.cpp
  physics/constraints/hardPhysicalConnection.cpp
  physics/constraints/motorConstraint.cpp
  physics/constraints/sinusoidalPistonConstraint.cpp

  physics/misc/serialization.cpp
  physics/misc/shapeLibrary.cpp
)
target_link_libraries(physics util)

add_executable(benchmarks
  benchmarks/benchmark.cpp
  benchmarks/basicWorld.cpp
  benchmarks/complexObjectBenchmark.cpp
  benchmarks/getBoundsPerformance.cpp
  benchmarks/manyCubesBenchmark.cpp
  benchmarks/worldBenchmark.cpp
)

target_link_libraries(benchmarks util)
target_link_libraries(benchmarks physics)

add_executable(tests 
  tests/testsMain.cpp

  tests/estimateMotion.cpp
  tests/testValues.cpp  

  tests/mathTests.cpp
  tests/motionTests.cpp
  tests/geometryTests.cpp
  tests/estimationTests.cpp
  tests/constraintTests.cpp
  tests/dataStructureTests.cpp
  tests/guiTests.cpp
  tests/indexedShapeTests.cpp
  tests/physicalStructureTests.cpp
  tests/physicsTests.cpp
)

target_link_libraries(tests util)
target_link_libraries(tests physics)

# install(TARGETS benchmarks DESTINATION bin)

