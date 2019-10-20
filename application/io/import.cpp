#include "core.h"

#include "import.h"
#include "../engine/io/import.h"

#include "../graphics/material.h"
#include "../graphics/visualShape.h"
#include "../util/stringUtil.h"
#include "partFactory.h"
#include <fstream>

/*
	WorldImport
*/

enum class Subject {
	PART = 2,
	CAMERA = 1,
	GLOBAL = 0,
	NONE = -1
};

void parseSubject(Subject subject, std::string path, std::map<std::string, std::string>& fields, std::map<std::string, PartFactory>& factories, World<ExtendedPart>& world, Screen& screen) {
	if (subject == Subject::PART) {
		Material material = Material();
		material.ambient = Import::parseVec4(fields.at("ambient"));
		material.diffuse = Import::parseVec3(fields.at("diffuse"));
		material.specular = Import::parseVec3(fields.at("specular"));
		material.reflectance = Import::parseFloat(fields.at("reflectance"));

		//! deprecated
		/*if (!fields.at("texture").empty())
			material.texture = Texture::load(fields.at("texture"));

		if (!fields.at("normals").empty())
			material.normal = Texture::load(fields.at("normals"));*/

		double density = Import::parseDouble(fields.at("density"));
		double friction = Import::parseDouble(fields.at("friction"));
		double bouncyness = Import::parseDouble(fields.at("bouncyness"));
		double mass = Import::parseDouble(fields.at("mass"));

		Position pos = Import::parsePosition(fields.at("position"));
		Mat3 rot = Import::parseMat3(fields.at("rotation"));

		std::string name = fields.at("name");
		int mode = Import::parseInt(fields.at("mode"));

		Vec3 velocity = Import::parseVec3(fields.at("velocity"));
		Vec3 angularvelocity = Import::parseVec3(fields.at("angularvelocity"));
		DiagonalMat3 inertia = Import::parseDiagonalMat3(fields.at("inertia"));

		bool anchored = Import::parseInt(fields.at("anchored"));

		std::string shapeReference = fields.at("shape");

		GlobalCFrame cframe(pos, rot);

		if (!factories.count(shapeReference)) {
			Log::info("Loading part %s in factory", (path + shapeReference).c_str());
			VisualShape shape = OBJImport::load(path + shapeReference);
			factories[shapeReference] = PartFactory(shape, screen, name);
		}

		ExtendedPart* part = factories.at(shapeReference).produce(cframe, { density, friction, bouncyness }, name);

		Physical physical = Physical(part, mass, inertia);
		physical.angularVelocity = angularvelocity;
		physical.velocity = velocity;

		part->parent = &physical;
		part->material = material;
		part->renderMode = mode;

		world.addPart(part, anchored);
	} else if (subject == Subject::CAMERA) {
		Position pos = Import::parsePosition(fields.at("position"));
		Mat3 rot = Import::parseMat3(fields.at("rotation"));
		double speed = Import::parseDouble(fields.at("speed"));
		double rspeed = Import::parseDouble(fields.at("rspeed"));
		bool fly = Import::parseInt(fields.at("flying"));

		screen.camera.cframe = GlobalCFrame(pos, rot);
		screen.camera.velocity = speed;
		screen.camera.angularVelocity = rspeed;
		screen.camera.flying = fly;
	} else if (subject == Subject::GLOBAL) {

	}

	fields.clear();
}

void loadBinaryWorld(std::string name, World<ExtendedPart>& world, Screen& screen) {

}

void loadNonBinaryWorld(std::string name, World<ExtendedPart>& world, Screen& screen) {
	Log::subject s(name);

	Subject subject = Subject::NONE;
	std::map<std::string, std::string> fields;
	std::map<std::string, PartFactory> factories;

	std::string path = std::string("./") + name + "/";
	std::string filename = path + "world.txt";
	std::ifstream input(filename.c_str());

	std::string line;
	while (getline(input, line)) {
		line = trim(line);
		if (line.empty() || line.at(0) == '#') {
			// next line
		} else {
			if (startWith(line, "part:")) {
				parseSubject(subject, path, fields, factories, world, screen);
				subject = Subject::PART;
				continue;
			}

			if (startWith(line, "camera:")) {
				parseSubject(subject, path, fields, factories, world, screen);
				subject = Subject::CAMERA;
				continue;
			}

			if (startWith(line, "global:")) {
				parseSubject(subject, path, fields, factories, world, screen);
				subject = Subject::GLOBAL;
				continue;
			}

			size_t pos = line.find("=");
			std::string property = rtrim(line.substr(0, pos));
			std::string value = ltrim(line.substr(pos + 1, line.length()));

			fields[property] = value;
		}
	}

	parseSubject(subject, path, fields, factories, world, screen);
}

void WorldImport::load(std::string name, World<ExtendedPart>& world, Screen& screen, bool binary) {
	if (binary)
		loadBinaryWorld(name, world, screen);
	else
		loadNonBinaryWorld(name, world, screen);
}

/*
	End of WorldImport
*/