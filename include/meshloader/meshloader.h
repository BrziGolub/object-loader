#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace meshloader {
	
	// Error handling structures
	enum class ErrorSeverity {
		Warning,
		Error
	};

	struct ObjError {
		size_t line = 0;
		std::string message;
		ErrorSeverity severity = ErrorSeverity::Error;

		std::string toString() const {
			const char* s = severity == ErrorSeverity::Warning ? "Warning" : "Error";
			if (line > 0) return std::string(s) + " (Line " + std::to_string(line) + "): " + message;
			return std::string(s) + ": " + message;
		}
	};

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

	Result loadOBJ(const std::string& path, Mesh& outMesh, std::vector<ObjError>& errors);
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

		if (std::getline(ss, part, '/')) idx.v = std::stoi(part);
		if (std::getline(ss, part, '/')) idx.vt = part.empty() ? -1 : std::stoi(part);
		if (std::getline(ss, part, '/')) idx.vn = part.empty() ? -1 : std::stoi(part);

		return idx;
	}

	bool resolveIndex(int& idx, int count) {
		if (idx > 0) {
			idx -= 1;
		}
		else if (idx < 0) {
			idx = count + idx;
		}
		else {
			// OBJ index 0 is not valid
			return false;
		}

		return idx >= 0 && idx < count;
	}

	Result loadOBJ(const std::string& path, Mesh& outMesh, std::vector<ObjError>& errors) {

		std::ifstream file(path);
		if (!file.is_open()) {
			return Result::FileNotFound;
		}

		using ObjFace = std::vector<ObjIndex>;

		std::vector<Vec3> positions;
		std::vector<Vec2> texcoords;
		std::vector<Vec3> normals;
		std::vector<ObjFace> faces;

		// Line number in .obj file of detected error
		std::string line;
		size_t lineNumber = 0;
		while (std::getline(file, line)) {
			++lineNumber;

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
					
					// Handling error with try catch
					// stoi() can throw
					ObjIndex idx;
					try {
						idx = parseIndex(token);
					}
					catch (...) {
						errors.push_back({ lineNumber, "Invalid face index format", ErrorSeverity::Error });
						continue;
					}
					
					// Resolve position index
					if (!resolveIndex(idx.v, (int)positions.size())) {
						errors.push_back({ lineNumber, "Position index out of range", ErrorSeverity::Error });
						continue;
					}

					// Resolve texcoord index
					if (idx.vt != -1) {
						if (!resolveIndex(idx.vt, (int)texcoords.size())) {
							errors.push_back({ lineNumber, "Texcoord index out of range", ErrorSeverity::Error });
							idx.vt = -1; // Degrade gracefully
						}
					}

					// Resolve normal index
					if (idx.vn != -1) {
						if (!resolveIndex(idx.vn, (int)normals.size())) {
							errors.push_back({ lineNumber, "Normal index out of range", ErrorSeverity::Error });
							idx.vn = -1; // Degrade gracefully
						}
					}

					face.push_back(idx);
				}

				// Preventing degenerate geometry early
				if (face.size() < 3) {
					errors.push_back({ lineNumber, "Face has fewer than 3 vertices", ErrorSeverity::Error });
					continue;
				}

				faces.push_back(face);
			}
		}

		// Empty file check
		if (positions.empty()) {
			errors.push_back({ 0, "OBJ file contains no vertex positions", ErrorSeverity::Error });
			return Result::ParseError;
		}
		if (faces.empty()) {
			errors.push_back({ 0, "OBJ file contains no faces", ErrorSeverity::Error });
			return Result::ParseError;
		}

		// Index bounds validation
		for (const ObjFace& face : faces) {
			for (const ObjIndex& idx : face) {

				if (idx.v < 0 || idx.v >= (int)positions.size()) {
					errors.push_back({ 0, "Position index out of range", ErrorSeverity::Error });
					return Result::ParseError;
				}

				if (idx.vt != -1 && (idx.vt < 0 || idx.vt >= (int)texcoords.size())) {
					errors.push_back({ 0, "Texcoord index out of range", ErrorSeverity::Error });
				}

				if (idx.vn != -1 && (idx.vn < 0 || idx.vn >= (int)normals.size())) {
					errors.push_back({ 0, "Normal index out of range", ErrorSeverity::Error });
				}
			}
		}

		// Deduplication + triangulation
		// Deduplication assumes indices are valid

		outMesh.vertices.clear();
		outMesh.indices.clear();
		
		std::unordered_map<ObjIndex, uint32_t, ObjIndexHash> vertexMap;

		auto validIndex = [&](int idx, int max) {
			return idx >= 0 && idx < max;
		};

		auto getVertexIndex = [&](const ObjIndex& idx) -> uint32_t {
			auto it = vertexMap.find(idx);
			if (it != vertexMap.end()) return it->second;

			Vertex vert{};

			// Valid index guard
			// This should never triggerif earlier validation is correct
			if (!validIndex(idx.v, positions.size())) {
				errors.push_back({ 0, "Invalid position index during mesh build", ErrorSeverity::Error });
				return 0;
			}

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

				// Degenerate triangle detection
				if (i0 == i1 || i1 == i2 || i0 == i2) {
					errors.push_back({ 0, "Degenerate triangle detected", ErrorSeverity::Error });
					continue;
				}

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

		if (!errors.empty()) return Result::ParseError;
		return Result::Success;
	}

}

#endif // MESHLOADER_IMPLEMENTATION
