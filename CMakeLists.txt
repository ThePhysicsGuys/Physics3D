cmake_minimum_required(VERSION 3.10)

project(Physics3D VERSION 1.9)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Building with: ${CMAKE_CXX_COMPILER_ID}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
	
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Oi /ot /GL")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
	
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
	
	#running benchmarks showed this to be a pessimization
	#set(CMAKE_INTERPROCEDURAL_OPTIMIZATION True)
	#surprisingly, also a pessimization
	#set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -ffast-math")
endif()

add_library(util STATIC 
  util/terminalColor.cpp
  util/log.cpp
  util/properties.cpp
  util/serializeBasicTypes.cpp
  util/stringUtil.cpp
  util/fileUtils.cpp
  util/valueCycle.cpp

  util/resource/resource.cpp
  util/resource/resourceLoader.cpp
  util/resource/resourceManager.cpp
)

if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
	target_link_libraries(util stdc++fs)
endif()

add_library(physics STATIC 
  physics/constraintGroup.cpp
  physics/debug.cpp
  physics/part.cpp
  physics/physical.cpp
  physics/physicsProfiler.cpp
  physics/rigidBody.cpp
  physics/layer.cpp
  physics/world.cpp
  physics/worldPhysics.cpp
  physics/inertia.cpp

  physics/math/linalg/eigen.cpp
  physics/math/linalg/largeMatrix.cpp
  physics/math/linalg/trigonometry.cpp

  physics/geometry/computationBuffer.cpp
  physics/geometry/convexShapeBuilder.cpp
  physics/geometry/genericIntersection.cpp
  physics/geometry/indexedShape.cpp
  physics/geometry/intersection.cpp
  physics/geometry/triangleMesh.cpp
  physics/geometry/polyhedron.cpp
  physics/geometry/shape.cpp
  physics/geometry/shapeBuilder.cpp
  physics/geometry/shapeClass.cpp
  physics/geometry/shapeCreation.cpp
  physics/geometry/builtinShapeClasses.cpp

  physics/datastructures/alignedPtr.cpp
  physics/datastructures/boundsTree.cpp

  physics/constraints/fixedConstraint.cpp
  physics/constraints/hardConstraint.cpp
  physics/constraints/hardPhysicalConnection.cpp
  physics/constraints/motorConstraint.cpp
  physics/constraints/controller/sineWaveController.cpp

  physics/misc/serialization.cpp
  physics/misc/shapeLibrary.cpp
  physics/misc/validityHelper.cpp
  physics/misc/filters/visibilityFilter.cpp
)
target_link_libraries(physics util)

add_executable(benchmarks
  benchmarks/benchmark.cpp
  benchmarks/basicWorld.cpp
  benchmarks/complexObjectBenchmark.cpp
  benchmarks/getBoundsPerformance.cpp
  benchmarks/manyCubesBenchmark.cpp
  benchmarks/worldBenchmark.cpp
  benchmarks/rotationBenchmark.cpp
  benchmarks/ecsBenchmark.cpp
)

target_link_libraries(benchmarks util)
target_link_libraries(benchmarks physics)

add_executable(tests 
  tests/testsMain.cpp

  tests/estimateMotion.cpp
  tests/testValues.cpp
  tests/generators.cpp

  tests/mathTests.cpp
  tests/rotationTests.cpp
  tests/motionTests.cpp
  tests/geometryTests.cpp
  tests/estimationTests.cpp
  tests/constraintTests.cpp
  tests/dataStructureTests.cpp
  tests/guiTests.cpp
  tests/indexedShapeTests.cpp
  tests/physicalStructureTests.cpp
  tests/physicsTests.cpp
  tests/inertiaTests.cpp
  tests/testFrameworkConsistencyTests.cpp
  tests/ecsTests.cpp
)

target_link_libraries(tests util)
target_link_libraries(tests physics)


find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Freetype REQUIRED)
find_package(Threads REQUIRED)

include_directories(PRIVATE "${GLFW_DIR}/include")
include_directories(PRIVATE "${GLEW_DIR}/include")
include_directories(PRIVATE "${FREETYPE_INCLUDE_DIRS}")
include_directories(PRIVATE "include")

add_library(imguiInclude STATIC
  include/imgui/imgui.cpp
  include/imgui/imgui_demo.cpp
  include/imgui/imgui_draw.cpp
  include/imgui/imgui_impl_glfw.cpp
  include/imgui/imgui_impl_opengl3.cpp
  include/imgui/imgui_widgets.cpp
)

add_library(graphics STATIC
  graphics/glfwUtils.cpp
  graphics/bindable.cpp
  graphics/core.cpp
  graphics/font.cpp
  graphics/meshLibrary.cpp
  graphics/renderer.cpp
  graphics/texture.cpp
  graphics/visualShape.cpp

  graphics/buffers/bufferLayout.cpp
  graphics/buffers/frameBuffer.cpp
  graphics/buffers/indexBuffer.cpp
  graphics/buffers/renderBuffer.cpp
  graphics/buffers/vertexArray.cpp
  graphics/buffers/vertexBuffer.cpp

  graphics/debug/guiDebug.cpp
  graphics/debug/profilerUI.cpp
  graphics/debug/visualDebug.cpp

  graphics/gui/color.cpp
  graphics/gui/container.cpp
  graphics/gui/gui.cpp
  graphics/gui/guiUtils.cpp
  graphics/gui/layout.cpp

  graphics/mesh/abstractMesh.cpp
  graphics/mesh/arrayMesh.cpp
  graphics/mesh/indexedMesh.cpp
  graphics/mesh/pointMesh.cpp
  graphics/mesh/vectorMesh.cpp

  graphics/path/path.cpp
  graphics/path/path3D.cpp

  graphics/resource/fontResource.cpp
  graphics/resource/shaderResource.cpp
  graphics/resource/textureResource.cpp

  graphics/shader/shader.cpp
  graphics/shader/shaderLexer.cpp
  graphics/shader/shaderParser.cpp
  graphics/shader/shaders.cpp
  graphics/shader/cshader.cpp
  graphics/shader/gshader.cpp
)

target_link_libraries(graphics imguiInclude)

target_include_directories(graphics PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/graphics")

add_library(engine STATIC
  engine/core.cpp
  engine/meshRegistry.cpp
  engine/visualData.cpp
  
  engine/ecs/component.cpp
  engine/ecs/entity.cpp
  engine/ecs/node.cpp
  engine/ecs/tree.cpp
  engine/ecs/registry.cpp

  engine/input/inputHandler.cpp
  engine/input/keyboard.cpp
  engine/input/modifiers.cpp
  engine/input/mouse.cpp

  engine/io/export.cpp
  engine/io/import.cpp

  engine/layer/layerStack.cpp

  engine/tool/toolManager.cpp

  engine/options/keyboardOptions.cpp

  engine/resource/meshResource.cpp
)
target_include_directories(engine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/engine")

target_link_libraries(engine graphics)

add_executable(application
  application/core.cpp
  application/application.cpp
  application/eventHandler.cpp
  application/extendedPart.cpp
  application/resources.cpp
  application/tickerThread.cpp
  application/worldBuilder.cpp
  application/worlds.cpp

  application/ecs/material.cpp
  application/ecs/mesh.cpp
  application/ecs/model.cpp
  application/ecs/transform.cpp
  application/ecs/components.h

  application/input/standardInputHandler.cpp

  application/io/saveDialog.cpp
  application/io/serialization.cpp

  application/layer/constraintLayer.cpp
  application/layer/debugLayer.cpp
  application/layer/debugOverlay.cpp
  application/layer/guiLayer.cpp
  application/layer/modelLayer.cpp
  application/layer/pickerLayer.cpp
  application/layer/postprocessLayer.cpp
  application/layer/skyboxLayer.cpp
  application/layer/testLayer.cpp
  application/layer/cameraLayer.cpp
  application/layer/shadowLayer.cpp
  application/layer/imguiLayer.cpp

  application/picker/editTools.cpp
  application/picker/selectionTool.cpp
  application/picker/picker.cpp
  application/picker/ray.cpp

  application/shader/shaders.cpp
  application/shader/basicShader.cpp
  application/shader/shaderBase.cpp

  application/view/camera.cpp
  application/view/frames.cpp
  application/view/screen.cpp
)
target_include_directories(application PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/application")


target_link_libraries(application util)
target_link_libraries(application physics)
target_link_libraries(application graphics)
target_link_libraries(application engine)

#target_link_libraries(application ${GLFW_LIBRARIES})
target_link_libraries(application glfw)
#target_link_libraries(application ${OPENGL_LIBRARIES})
target_link_libraries(application OpenGL::GL)
#target_link_libraries(application ${GLEW_LIBRARIES})
target_link_libraries(application GLEW::GLEW)
target_link_libraries(application ${FREETYPE_LIBRARIES})
target_link_libraries(application Threads::Threads)

# install(TARGETS benchmarks DESTINATION bin)

