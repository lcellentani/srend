#pragma once

#include <memory>
#include <vector>

#include "glm/glm.hpp"

#include "tgaimage.h"

class Model {
public:
	Model();
	~Model();

	bool Load(const char* filename);
	
	size_t NumberOfVetices() const;
	size_t NumberOfFaces() const;

	glm::vec3 GetVertex(const int index) const;
	glm::vec3 GetNormal(const int index) const;
	glm::vec2 GetTexcoord0(const int index) const;

	std::vector<int> GetFace(const int index) const;

	std::shared_ptr<TGAImage> GetDiffuseMap() const;

private:
	std::shared_ptr<TGAImage> loadTexture(const std::string& filename, const std::string& suffix);

private:
	std::vector<glm::vec3> mVertices;
	std::vector<glm::vec3> mNormals;
	std::vector<glm::vec2> mTexcoord0;
	std::vector<std::vector<int>> mFaces;

	std::shared_ptr<TGAImage> mDiffuseMap;
};