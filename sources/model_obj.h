#pragma once

#include <vector>
#include <glm/vec3.hpp>

class Model {
public:
	Model();
	~Model();

	bool Load(const char* filename);
	
	size_t NumberOfVetices() const;
	size_t NumberOfFaces() const;

	glm::vec3 GetVertex(const int index) const;
	std::vector<int> GetFace(const int index) const;

private:
	std::vector<glm::vec3> mVertices;
	std::vector<std::vector<int>> mFaces;
};