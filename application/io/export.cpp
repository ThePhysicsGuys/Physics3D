#include "core.h"

#include "export.h"

#include "../visualShape.h"

#include <fstream>
#include <sstream>
#include <direct.h>

/*
	Export
*/

template<typename T>
void Export::write(std::ostream& output, T const * buffer, int size) {
	for (int i = 0; i < size; i++) {
		output.write((T*)(buffer + i * sizeof(T)), 1);
	}
}

template<typename T>
void Export::write(std::ostream& output, const T& value) {
	char const * v = reinterpret_cast<char const *>(&value);
	write<char>(output, v, sizeof(T));
}

std::string Export::str(Vec3 vector) {
	std::stringstream ss;
	ss << vector.x << " " << vector.y << " " << vector.z;
	return ss.str();
}

std::string Export::str(Vec4 vector) {
	std::stringstream ss;
	ss << vector.x << " " << vector.y << " " << vector.z << " " << vector.w;
	return ss.str();
}

std::string Export::str(Mat3 matrix) {
	std::stringstream ss;

	ss << matrix.m00 << " " << matrix.m01 << " " << matrix.m02 << " ";
	ss << matrix.m10 << " " << matrix.m11 << " " << matrix.m12 << " ";
	ss << matrix.m20 << " " << matrix.m21 << " " << matrix.m22;

	return ss.str();
}

std::string Export::str(Position pos) {
	std::stringstream ss;
	ss << pos.x.value << " " << pos.y.value << " " << pos.z.value;
	return ss.str();
}

std::string Export::str(DiagonalMat3 matrix) {
	return Export::str(Vec3(matrix.m[0], matrix.m[1], matrix.m[2]));
}

std::string Export::str(int num) {
	return std::to_string(num);
}

std::string Export::str(double num) {
	return std::to_string(num);
}

/*
	End of Export
*/



/*
	OBJExport
*/

void saveBinaryObj(std::string filename, const VisualShape& shape) {
	struct stat buffer;

	if (stat(filename.c_str(), &buffer) != -1) {
		Log::warn("File already exists: %s", filename.c_str());
	}

	std::ofstream output;

	output.open(filename, std::ios::binary | std::ios::out);

	char V = 0;
	char VN = 1;
	char VT = 2;
	char VNT = 3;

	char flag = V;

	if (shape.uvs != nullptr && shape.normals != nullptr) {
		flag = VNT;
	} else if (shape.uvs != nullptr) {
		flag = VT;
	} else if (shape.normals != nullptr) {
		flag = VN;
	}

	Export::write<char>(output, flag);
	Export::write<const int>(output, shape.vertexCount);
	Export::write<const int>(output, shape.triangleCount);

	int i = 0;
	for (Vec3f vertex : shape.iterVertices()) {

		Export::write<Vec3f>(output, vertex);
		i += 1;
	}

	if (shape.normals != nullptr) {
		for (int i = 0; i < shape.vertexCount; i++) {
			Vec3f normal = shape.normals.get()[i];
			Export::write<Vec3f>(output, normal);
		}
	}

	if (shape.uvs != nullptr) {
		for (int i = 0; i < shape.vertexCount; i++) {
			Vec2f uv = shape.uvs.get()[i];
			Export::write<Vec2f>(output, uv);
		}
	}

	for (Triangle triangle : shape.iterTriangles()) {
		for (int index : triangle.indexes) {
			Export::write<int>(output, index);
		}
	}

	output.close();
}

void saveNonBinaryObj(const std::string& filename, const VisualShape& shape) {
	struct stat buffer;

	if (stat(filename.c_str(), &buffer) != -1) {
		Log::warn("File already exists: %s", filename.c_str());
	}

	std::ofstream output;
	output.open(filename);

	for (Vec3f vertex : shape.iterVertices()) {
		output << "v " << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
	}

	if (shape.normals != nullptr) {
		for (int i = 0; i < shape.vertexCount; i++) {
			Vec3 normal = shape.normals.get()[i];
			output << "vn " << normal.x << " " << normal.y << " " << normal.z << std::endl;
		}
	}

	if (shape.uvs != nullptr) {
		for (int i = 0; i < shape.vertexCount; i++) {
			Vec2 uv = shape.uvs.get()[i];
			output << "vt " << uv.x << " " << uv.y << std::endl;
		}
	}

	for (Triangle triangle : shape.iterTriangles()) {
		output << "f";

		for (int index : triangle.indexes) {
			index++;
			if (shape.normals != nullptr && shape.uvs != nullptr) {
				output << " " << index << "/" << index << "/" << index;
			} else if (shape.uvs != nullptr) {
				output << " " << index << "//" << index;
			} else if (shape.normals != nullptr) {
				output << " " << index << "/" << index;
			} else {
				output << " " << index;
			}
		}

		output << std::endl;
	}

	output.close();
}

void OBJExport::save(const std::string& filename, const VisualShape& shape, bool binary) {
	if (binary)
		saveBinaryObj(filename, shape);
	else
		saveNonBinaryObj(filename, shape);
}

/*
	End of OBJExport
*/



/*
	WorldExport
*/

void savePart(std::ofstream& output, const ExtendedPart& part, std::string shape, bool anchored) {
	output << "part:" << std::endl;
	output << "\tanchored = " << anchored << std::endl;
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
	output << "\tvelocity = " << Export::str(part.parent->velocity) << std::endl;
	output << "\tangularvelocity = " << Export::str(part.parent->angularVelocity) << std::endl;
	output << "\tmass = " << Export::str(part.parent->mass) << std::endl;
	output << "\tinertia = " << Export::str(part.parent->inertia) << std::endl;
}

void saveCamera(std::ofstream& output, Camera& camera) {
	output << "camera:" << std::endl;
	output << "\tposition = " << Export::str(camera.cframe.position) << std::endl;
	output << "\trotation = " << Export::str(camera.cframe.rotation) << std::endl;
	output << "\tspeed = " << Export::str(camera.speed) << std::endl;
	output << "\trspeed = " << Export::str(camera.rspeed) << std::endl;
	output << "\tflying = " << Export::str(camera.flying) << std::endl;
}

void saveGlobals(std::ofstream& output, World<ExtendedPart>& world) {
	output << "global:" << std::endl;
	output << "\tfreeparts = " << world.getPartCount() << std::endl;
	output << "\tanchoredparts = " << world.getAnchoredPartCount() << std::endl;
}

void saveBinaryWorld(const std::string& name, World<ExtendedPart>& world, Camera& camera) {

}

void saveNonBinaryWorld(const std::string& name, World<ExtendedPart>& world, Camera& camera) {
	Log::setSubject(name);
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

	for (const Part& part : world.iterAnchoredParts()) {
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

		savePart(output, extendedPart, shape, true);
	}

	for (const Part& part : world.iterFreeParts()) {
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

		savePart(output, extendedPart, shape, false);
	}

	Log::info("World exported");
	Log::resetSubject();

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