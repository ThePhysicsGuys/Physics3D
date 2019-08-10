#pragma once

class IndexedMesh;

namespace Library {
	extern IndexedMesh* cube;
	extern IndexedMesh* sphere;
	
	void init();
	void close();
}