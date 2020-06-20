#include "core.h"

#include "resources.h"

#include "../util/resource/resourceDescriptor.h"

#ifdef _MSC_VER
#include "resource.h"
ResourceDescriptor applicationResources[] {
	{ IDR_SHADER1, "SHADER" },
	{ IDR_SHADER2, "SHADER" },
	{ IDR_SHADER3, "SHADER" },
	{ IDR_SHADER4, "SHADER" },
	{ IDR_SHADER5, "SHADER" },
	{ IDR_SHADER6, "SHADER" },
	{ IDR_SHADER7, "SHADER" },
	{ IDR_SHADER8, "SHADER" },
	{ IDR_SHADER9, "SHADER" },
	{ IDR_SHADER10, "SHADER" },
	{ IDR_SHADER11, "SHADER" },
	{ IDR_SHADER12, "SHADER" },
	{ IDR_SHADER13, "SHADER" },
	{ IDR_SHADER14, "SHADER" },
	{ IDR_SHADER15, "SHADER" },
	{ IDR_SHADER16, "SHADER" },
	{ IDR_SHADER17, "SHADER" },
	{ IDR_SHADER18, "SHADER" },
	{ IDR_OBJ1, "OBJ" },
	{ IDR_OBJ2, "OBJ" }
};
#else

ResourceDescriptor applicationResources[] {
	{ "../res/shaders/basic.shader"       , "SHADER" },
	{ "../res/shaders/vector.shader"      , "SHADER" },
	{ "../res/shaders/origin.shader"      , "SHADER" },
	{ "../res/shaders/font.shader"        , "SHADER" },
	{ "../res/shaders/depth.shader"       , "SHADER" },
	{ "../res/shaders/quad.shader"        , "SHADER" },
	{ "../res/shaders/postprocess.shader" , "SHADER" },
	{ "../res/shaders/skybox.shader"      , "SHADER" },
	{ "../res/shaders/mask.shader"        , "SHADER" },
	{ "../res/shaders/point.shader"       , "SHADER" },
	{ "../res/shaders/test.shader"        , "SHADER" },
	{ "../res/shaders/blur.shader"        , "SHADER" },
	{ "../res/shaders/line.shader"        , "SHADER" },
	{ "../res/shaders/instance.shader"    , "SHADER" },
	{ "../res/shaders/sky.shader"         , "SHADER" },
	{ "../res/shaders/lighting.shader"    , "SHADER" },
	{ "../res/shaders/debug.shader"       , "SHADER" },
	{ "../res/shaders/depthbuffer.shader" , "SHADER" },
	{ "../res/models/stall.obj"           , "OBJ" },
	{ "../res/models/sphere.obj"          , "OBJ" }
};

#endif