#include "meshloader/meshloader.h"

#include <cassert>
#include <iostream>

void test_triangle() {

	meshloader::Mesh mesh;
	std::string path = std::string(TEST_ASSETS_DIR) + "/triangle.obj";
	auto result = meshloader::loadOBJ(path, mesh);

	assert(result == meshloader::Result::Success);
	//assert(mesh.vertices.size() == 3);
	//assert(mesh.indices.size() == 3);
}