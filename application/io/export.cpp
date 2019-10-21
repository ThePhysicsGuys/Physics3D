#include "core.h"

#include "export.h"
#include "../engine/io/export.h"

#include <fstream>
#include <sstream>
#include <direct.h>

#include "../graphics/visualShape.h"
#include "extendedPart.h"
#include "view/camera.h"

void savePart(std::ofstream& output, const ExtendedPart& part, std::string shape) {
	output << "part:" << std::endl;
	output << "\ttexture = " << "" << std::endl;
	output << "\tnormals = " << "" << std::endl;
	output << "\tambient = " << Export::str(part.material.ambient) << std::endl;
	output << "\tdiffuse = " << Export::str(part.material.diffuse) << std::endl;
	output << "\tspecular = " << Export::str(part.material.specular) << std::endl;
	output << "\treflectance = " << Export::str(part.material.reflectance) << std::endl;
	output << "\tshape = " << shape << std::endl;
	output << "\tname = " << part.name << std::endl;
	output << "\tmode = " << Export::str(part.renderMode) << std::endl;
	output << "\tposition = " << Export::str(part.getCFrame().position) << std::endl;
	output << "\trotation = " << Export::str(part.getCFrame().rotation) << std::endl;
	output << "\tdensity = " << Export::str(part.properties.density) << std::endl;
	output << "\tfriction = " << Export::str(part.properties.friction) << std::endl;
	output << "\tbouncyness = " << Export::str(part.properties.bouncyness) << std::endl;
	output << "\tvelocity = " << Export::str(part.parent->velocity) << std::endl;
	output << "\tangularvelocity = " << Export::str(part.parent->angularVelocity) << std::endl;
	output << "\tmass = " << Export::str(part.parent->mass) << std::endl;
	output << "\tinertia = " << Export::str(part.parent->inertia) << std::endl;
}

void saveCamera(std::ofstream& output, Camera& camera) {
	output << "camera:" << std::endl;
	output << "\tposition = " << Export::str(camera.cframe.position) << std::endl;
	output << "\trotation = " << Export::str(camera.cframe.rotation) << std::endl;
	output << "\tspeed = " << Export::str(camera.velocity) << std::endl;
	output << "\trspeed = " << Export::str(camera.angularVelocity) << std::endl;
	output << "\tflying = " << Export::str(camera.flying) << std::endl;
}

void saveGlobals(std::ofstream& output, World<ExtendedPart>& world) {
	output << "global:" << std::endl;
	output << "\tparts = " << world.getPartCount() << std::endl;
}

void saveBinaryWorld(const std::string& name, World<ExtendedPart>& world, Camera& camera) {

}

void saveNonBinaryWorld(const std::string& name, World<ExtendedPart>& world, Camera& camera) {
	Log::subject s(name);
	Log::info("Exporting world...");

	std::string folder = std::string("./") + name;
	std::string file = folder + "/world.txt";

	int result = _mkdir(folder.c_str());

	std::ofstream output;

	output.open(file, std::ofstream::out | std::ofstream::trunc);

	Log::info("Exporting globals");
	saveGlobals(output, world);

	Log::info("Exporting camera");
	saveCamera(output, camera);

	std::map<int, std::string> shapes;

	for (const Part& part : world) {
		const ExtendedPart& extendedPart = static_cast<const ExtendedPart&>(part);

		std::string shape;
		if (shapes.count(extendedPart.drawMeshId)) {
			shape = shapes.at(extendedPart.drawMeshId);
		} else {
			shape = "part_" + Export::str(extendedPart.drawMeshId);
			shapes[extendedPart.drawMeshId] = shape;

			Log::info("Exporting %s", shape.c_str());
			OBJExport::save(folder + "/" + shape + ".obj", extendedPart.visualShape);
		}

		savePart(output, extendedPart, shape);
	}

	Log::info("World exported");

	output.close();
}

void WorldExport::save(const std::string& name, World<ExtendedPart>& world, Camera& camera, bool binary) {
	if (binary)
		saveBinaryWorld(name, world, camera);
	else
		saveNonBinaryWorld(name, world, camera);
}

/*
	End of WorldExport
*/