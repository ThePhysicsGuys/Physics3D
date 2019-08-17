#pragma once

class IndexedMesh;

namespace Library {
	extern IndexedMesh* cube;
	extern IndexedMesh* sphere;
	extern IndexedMesh* vector;
	
	void init();
	void close();
}