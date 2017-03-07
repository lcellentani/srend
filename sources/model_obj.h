#pragma once

#include <memory>
#include <vector>
#include <cstdint>

#include "vector.h"
#include "tgaimage.h"

namespace srend
{

class Model {
public:
	Model();
	~Model();

	bool Load(const char* filename);

	size_t NumberOfVetices() const;
	size_t NumberOfFaces() const;

	size_t GetFaceVerticesCount(const int findex) const;

	vec3f GetVertex(const int findex, const int vindex) const;
	vec3f GetNormal(const int findex, const int vindex) const;
	vec2f GetTexcoord0(const int findex, const int vindex) const;

	std::vector<uint32_t> GetFace(const int index) const;

	std::shared_ptr<TGAImage> GetDiffuseMap() const;

private:
	std::shared_ptr<TGAImage> loadTexture(const std::string& filename, const std::string& suffix);

private:
	std::vector<vec3f> mVertices;
	std::vector<vec3f> mNormals;
	std::vector<vec2f> mTexcoord0;
	std::vector<std::vector<uint32_t>> mFaces;

	std::shared_ptr<TGAImage> mDiffuseMap;
};

} // namespace srend