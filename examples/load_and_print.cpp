#define MESHLOADER_IMPLEMENTATION
#include "meshloader/meshloader.h"

#include <iostream>

void load_and_print(meshloader::Mesh& mesh);

int main() {

	meshloader::Mesh mesh;
	std::vector<meshloader::ObjError> errors;

	if (meshloader::loadOBJ(EXAMPLE_ASSETS_DIR "/triangle.obj", mesh, errors) != meshloader::Result::Success) {
		std::cerr << "Failed to load mesh" << std::endl;
		return 1;
	}

	load_and_print(mesh);

	return 0;
}

void load_and_print(meshloader::Mesh& mesh) {
	
	std::cout << "Vertices: " << mesh.vertices.size() << std::endl;

	for each (meshloader::Vertex vert in mesh.vertices)
	{
		std::cout << "x=" << vert.position.x << "\ty=" << vert.position.y << "\tz=" << vert.position.z << std::endl;
	}

	std::cout << "Indices: " << mesh.indices.size() << std::endl;
}