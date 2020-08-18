#pragma once


namespace P3D::Graphics {

class IndexedMesh;

namespace Library {
extern IndexedMesh* cube;
extern IndexedMesh* sphere;

void onInit();
void onClose();
}

};