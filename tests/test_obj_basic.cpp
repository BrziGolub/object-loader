#include "meshloader/meshloader.h"

#include <cassert>
#include <iostream>

bool sanity_test() {
	
	meshloader::Mesh mesh;
	std::vector<meshloader::ObjError> errors;
	meshloader::Result r = meshloader::loadOBJ(TEST_ASSETS_DIR "/sanity.obj", mesh, errors);

	return r == meshloader::Result::Success;
}

void test_triangle() {

	meshloader::Mesh mesh;
	std::vector<meshloader::ObjError> errors;

	auto result = meshloader::loadOBJ(TEST_ASSETS_DIR "/triangle.obj", mesh, errors);

	assert(result == meshloader::Result::Success);
	assert(mesh.vertices.size() == 3);
	assert(mesh.indices.size() == 3);
}