#include "meshloader/meshloader.h"

#include <cassert>

#define EXPECT_TRUE(x) if (!(x)) return false;
#define EXPECT_EQ(a,b) if ((a)!=(b)) return false;

bool sanity_test() {
	
	meshloader::Mesh mesh;
	std::vector<meshloader::ObjError> errors;
	meshloader::Result r = meshloader::loadOBJ(TEST_ASSETS_DIR "/sanity.obj", mesh, errors);

	return r == meshloader::Result::Success;
}

bool test_negative_indices() {

	meshloader::Mesh mesh;
	std::vector<meshloader::ObjError> errors;

	meshloader::Result r = meshloader::loadOBJ(TEST_ASSETS_DIR "/negative_indices.obj", mesh, errors);
	
	EXPECT_EQ(r, meshloader::Result::Success);
	EXPECT_TRUE(errors.empty());
	EXPECT_EQ(mesh.vertices.size(), 3);
	EXPECT_EQ(mesh.indices.size(), 3);

	return true;
}

void test_triangle() {

	meshloader::Mesh mesh;
	std::vector<meshloader::ObjError> errors;

	auto result = meshloader::loadOBJ(TEST_ASSETS_DIR "/triangle.obj", mesh, errors);

	assert(result == meshloader::Result::Success);
	assert(mesh.vertices.size() == 3);
	assert(mesh.indices.size() == 3);
}