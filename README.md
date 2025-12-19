# Mesh Loader - Minimal OBJ Loader

Mesh loader is a header-only Wavefront OBJ loader written in modern C++(C++17). 
It is designed as a small, low-level, dependecy-free library suitable for graphics engines, 
OpenGL/Vulkan projects, and academic work.

The goal of this project is correctness, robustness, and clarity, rather than supporting 
every feature of the OBJ format.

---
## Features Implemented So Far

1. **Header-only design**

	- Single include file
	- Optional implementation via `#define MESHLOADER_IMPLEMENTATION`
	- Easy to integrate into any C++ project
	```cpp
	#define MESHLOADER_IMPLEMENTATION
	#include "meshloader/meshloader.h"
	```

2. **Core mesh structures**

	The loader outputs a GPU-ready indexed mesh:
	```cpp
	struct Vertex {
		Vec3 position;
		Vec3 normal;
		Vec2 texcoord;
	};
 
	struct Mesh {
		std::vector<Vertex> vertices;
		std::vector<std::uint32_t> indices;
	};
	```

	- Suitable for OpenGL / Vulcn / DirectX
	- Indexed drawing (`glDrawElements` style)

3. **OBJ parsing support**

	Supported OBJ elemnts:
	- `v` - vertex positions
	- `vt` - texture coordinates
	- `vn` - vertex normals
	- `f` - polygonal faces (triangles, quads, n-gons)
	
	Unsupported or ignored elements:
	- materials (`mtl`, `usemtl`)
	- groups / objects (`g`, `o`)
	- smoothing groups

4. **Fan triangulation**

	Polygonal faces are triangulated using fan triangulation:
	```
	f v0 v1 v2 v3 v4
	
	→ (v0, v1, v2)
	→ (v0, v2, v3)
	→ (v0, v3, v4)
	```
	This allows arbitrary n-gons to be converted into triangles.

5. **Vertex deduplication (correct OBJ semantics)**

	OBJ uses **separate indices** for positions, normals, and texture coordinates. Mesh loader correctly treats a vertex as a unique combination of:

	`(position index, texcoord index, normal index)`
	
	Implementation details:
	- Hash-based deduplication using `std::unordered_map`
	- Reuses vertices when the full (v, vt, vn) triple matches
	- Produces a compact vertex buffer and index buffer

	This behaviour matches real-world rendering pipelines.

6. **Correct negative index handling**

	Negative indices are fully supported according to the OBJ specification:
	
	| OBJ index | Meaning |
	| :- | :- |
	| `1` | first element |
	| `-1` | last defined element |
	| `-2` | second-to-last |

	Rules applied:

	- Indices are resolved at **parse time**
	- index `0` is rejected (invalid in OBJ)
	- Out-of-range negative indices are detected as errors

	This ensures compatibility with real-world OBJ files.
1. **Robust error handling and validation**

	The loader performs **defensive validation** and never crashes on malformed input.

	Detected errors include:

	- File not found
	- Empty OBJ files (no geometry)
	- Faces with fewer than 3 vertices
	- Invalid or non-numeric face indices
	- Out-of-range position / normal / texcoord indices
	- Degenerate triangles

	Errors are reported via a structure error list:

	```cpp
	struct ObjError {
		size_t line;			// Line number (0 if not applicable)
		std::string message;		// human-readable description
		ErrorSeverity severity;		// enum (Error, Warning)
	
		std::string toString() const;
	};
	```

	The loader returns a `Result` enum:

	```cpp
	enum class Result {
		Success,
		FileNotFound,
		ParseError,
		Unsupported
	};
	```
1. **Graceful degradation**

	- Missing normals or UVs are allowed
	- Invalid optional indices degrade safely
	- Fatal errors stop loading early

	This allows the loader to handle imperfect but usable OBJ files.

---
## Build Requirements

- C++17 compatible compiler
- Tested with:
	- MSVC (Visual Studio)

No external dependencies.

---
## Licence

This project is provided for educational use.

---
**Status:** Actively developed