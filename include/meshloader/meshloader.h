#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace meshloader {
	
	struct Vec2 {
		float x = 0.0f;
		float y = 0.0f;
	};

	struct Vec3 {
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};

	struct Vertex {
		Vec3 position;
		Vec3 normal;
		Vec2 texcoord;
	};

	struct Mesh {
		std::vector<Vertex> vertices;
		std::vector<std::uint32_t> indices;
	};

	enum class Result {
		Success,
		FileNotFound,
		ParseError,
		Unsupported
	};

	Result loadOBJ(const std::string& path, Mesh& outMesh);
}

#ifdef MESHLOADER_IMPLEMENTATION

// ============== IMPLEMENTATION ==============

// Includes
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace meshloader {

	// Internal helpers go here

	Result loadOBJ(const std::string& path, Mesh& outMesh) {

		std::ifstream file(path);
		if (!file.is_open()) {
			return Result::FileNotFound;
		}

		// TODO: Implement functionality of object loader

		return Result::Success;
	}

}

#endif // MESHLOADER_IMPLEMENTATION
