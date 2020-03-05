#pragma once


namespace Graphics {

class IndexedMesh;

namespace Library {
extern IndexedMesh* cube;
extern IndexedMesh* sphere;
extern IndexedMesh* vector;

void onInit();
void onClose();
}

};