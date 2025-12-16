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

#include <fstream>
#include <sstream>
#include <unordered_map>

namespace meshloader {

	struct ObjIndex {
		int v = -1;
		int vt = -1;
		int vn = -1;

		bool operator==(const ObjIndex& other) const {
			return v == other.v && vt == other.vt && vn == other.vn;
		}
	};

	struct ObjIndexHash {
		std::size_t operator()(const ObjIndex& idx) const noexcept {
			std::size_t h1 = std::hash<int>{}(idx.v);
			std::size_t h2 = std::hash<int>{}(idx.vt);
			std::size_t h3 = std::hash<int>{}(idx.vn);

			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};

	ObjIndex parseIndex(const std::string& token) {
		ObjIndex idx;
		
		std::stringstream ss(token);
		std::string part;

		if (std::getline(ss, part, '/')) idx.v = std::stoi(part) - 1;
		if (std::getline(ss, part, '/')) idx.vt = part.empty() ? -1 : std::stoi(part) - 1;
		if (std::getline(ss, part, '/')) idx.vn = part.empty() ? -1 : std::stoi(part) - 1;

		return idx;
	}

	Result loadOBJ(const std::string& path, Mesh& outMesh) {

		std::ifstream file(path);
		if (!file.is_open()) {
			return Result::FileNotFound;
		}

		using ObjFace = std::vector<ObjIndex>;

		std::vector<Vec3> positions;
		std::vector<Vec2> texcoords;
		std::vector<Vec3> normals;
		std::vector<ObjFace> faces;

		std::string line;
		while (std::getline(file, line)) {

			if (line.empty() || line[0] == '#') continue;

			std::istringstream iss(line);
			std::string type;
			iss >> type;

			if (type == "v") {
				Vec3 v;
				iss >> v.x >> v.y >> v.z;
				positions.push_back(v);
			}
			else if (type == "vt") {
				Vec2 vt;
				iss >> vt.x >> vt.y;
				texcoords.push_back(vt);
			}
			else if (type == "vn") {
				Vec3 n;
				iss >> n.x >> n.y >> n.z;
				normals.push_back(n);
			}
			else if (type == "f") {
				ObjFace face;

				std::string token;
				while (iss >> token) {
					ObjIndex idx = parseIndex(token);
					
					if (idx.v < 0) idx.v += positions.size();

					face.push_back(idx);
				}

				if (face.size() < 3) return Result::ParseError;

				faces.push_back(face);
			}
		}

		// Deduplication + triangulation

		outMesh.vertices.clear();
		outMesh.indices.clear();
		
		std::unordered_map<ObjIndex, uint32_t, ObjIndexHash> vertexMap;

		auto getVertexIndex = [&](const ObjIndex& idx) -> uint32_t {
			auto it = vertexMap.find(idx);
			if (it != vertexMap.end()) return it->second;

			Vertex vert{};

			vert.position = positions[idx.v];

			if (idx.vt >= 0 && idx.vt < (int)texcoords.size()) vert.texcoord = texcoords[idx.vt];

			if (idx.vn >= 0 && idx.vn < (int)normals.size()) vert.normal = normals[idx.vn];

			uint32_t newIndex = static_cast<uint32_t>(outMesh.vertices.size());
			outMesh.vertices.push_back(vert);
			vertexMap[idx] = newIndex;

			return newIndex;
		};

		// Triangulate and build index buffer

		for (const ObjFace& face : faces) {
			// Fan triangulation
			for (size_t i = 1; i + 1 < face.size(); ++i) {
				// indices:
				// 0 1 2
				// 0 2 3
				// 0 3 4
				// ...
				uint32_t i0 = getVertexIndex(face[0]);
				uint32_t i1 = getVertexIndex(face[i]);
				uint32_t i2 = getVertexIndex(face[i + 1]);

				outMesh.indices.push_back(i0);
				outMesh.indices.push_back(i1);
				outMesh.indices.push_back(i2);
			}
		}

		/*
		* Complexity:
		*	Time: O(n) average
		*	Memory: O(v) unique vertices
		*	Uses hash-based deduplication
		*/

		return Result::Success;
	}

}

#endif // MESHLOADER_IMPLEMENTATION
