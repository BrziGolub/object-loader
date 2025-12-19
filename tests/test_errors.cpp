#include "meshloader/meshloader.h"

#include <iostream>

#define EXPECT_TRUE(x) if (!(x)) return false;
#define EXPECT_EQ(a,b) if ((a)!=(b)) return false;

bool test_invalid_index() {
	meshloader::Mesh mesh;
	std::vector<meshloader::ObjError> errors;

	meshloader::Result r = meshloader::loadOBJ(TEST_ASSETS_DIR "/bad_index.obj", mesh, errors);

	EXPECT_EQ(r, meshloader::Result::ParseError);
	EXPECT_TRUE(!errors.empty());

	std::string msg = errors[0].toString();
	EXPECT_TRUE(msg.find("index") != std::string::npos);

	return true;
}

bool test_short_face() {
	meshloader::Mesh mesh;
	std::vector<meshloader::ObjError> errors;

	meshloader::Result r = meshloader::loadOBJ(TEST_ASSETS_DIR "/short_face.obj", mesh, errors);

	EXPECT_EQ(r, meshloader::Result::ParseError);
	EXPECT_TRUE(!errors.empty());

	std::string msg = errors[0].toString();
	EXPECT_TRUE(msg.find("fewer than 3") != std::string::npos);

	return true;
}

bool test_invalid_token() {
	meshloader::Mesh mesh;
	std::vector<meshloader::ObjError> errors;

	meshloader::Result r = meshloader::loadOBJ(TEST_ASSETS_DIR "/invalid_token.obj", mesh, errors);

	EXPECT_EQ(r, meshloader::Result::ParseError);
	EXPECT_TRUE(!errors.empty());

	std::string msg = errors[0].toString();
	EXPECT_TRUE(msg.find("Invalid") != std::string::npos);

	return true;
}

bool test_empty_file() {
	meshloader::Mesh mesh;
	std::vector<meshloader::ObjError> errors;

	meshloader::Result r = meshloader::loadOBJ(TEST_ASSETS_DIR "/empty.obj", mesh, errors);

	EXPECT_EQ(r, meshloader::Result::ParseError);
	EXPECT_TRUE(!errors.empty());

	return true;
}