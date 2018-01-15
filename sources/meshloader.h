#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "glm/vec3.hpp"
#include "glm//vec2.hpp"

namespace srend
{

class Model final {
public:
	struct Point {
		glm::vec3 mPosition;
		glm::vec3 mNormal;
		glm::vec2 mTexcoord;
	};

	struct Face {
		std::vector<Point> mVertices;
	};

	struct Shape {
		std::vector<Face> mFaces;
	};

public:
	Model();
	~Model();

	bool IsValid() const;

	std::size_t GetNumberOfShapes() const;
	const Shape& GetShape(uint32_t index) const;

	bool Load(const char* filename, bool triangulate = true);

private:
	std::vector<Shape> mShapes;
};

} // namespace srend