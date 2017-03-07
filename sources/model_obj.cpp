#include "model_obj.h"

#include <fstream>
#include <string>
#include <sstream>
#include <cassert>

namespace srend
{

Model::Model() : mVertices(), mNormals(), mTexcoord0(), mFaces() {

}

Model::~Model() {

}

bool Model::Load(const char* filename) {
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (!in.fail()) {
		while (!in.eof()) {
			std::string line;
			std::getline(in, line);
			if (!line.empty()) {
				std::istringstream iss(line.c_str());
				if (line.compare(0, 2, "v ") == 0) {
					char discard_v;
					vec3f vertex;

					iss >> discard_v;

					iss >> vertex.x();
					iss >> vertex.y();
					iss >> vertex.z();

					mVertices.push_back(vertex);

					//iss >> trash;
					//Vec3f v;
					//for (int i = 0; i<3; i++) iss >> v[i];
					//verts_.push_back(v);

				}
				else if (line.compare(0, 3, "vn ") == 0) {
					char discard_f;
					vec3f normal;

					iss >> discard_f;
					iss >> discard_f;

					iss >> normal.x();
					iss >> normal.y();
					iss >> normal.z();

					mNormals.push_back(normal);
				}
				else if (line.compare(0, 3, "vt ") == 0) {
					char discard_f;
					vec2f uv;

					iss >> discard_f;
					iss >> discard_f;

					iss >> uv.x();
					iss >> uv.y();

					mTexcoord0.push_back(uv);
				}
				else if (line.compare(0, 2, "f ") == 0) {
					char discard_f;
					std::vector<uint32_t> indices;

					iss >> discard_f;

					char discard;
					int vtxIndex, texIndex, normIndex;
					while (iss >> vtxIndex >> discard >> texIndex >> discard >> normIndex) {
						vtxIndex--; // in wavefront obj all indices start at 1, not zero
						texIndex--;
						normIndex--;
						indices.push_back(vtxIndex);
						indices.push_back(texIndex);
						indices.push_back(normIndex);
					}
					mFaces.push_back(indices);
				}
			}
		}

		std::string diffuseSuffix("_diffuse.tga");
		mDiffuseMap = loadTexture(filename, diffuseSuffix);

		return true;
	}
	return false;
}

size_t Model::NumberOfVetices() const {
	return mVertices.size();
}

size_t Model::NumberOfFaces() const {
	return mFaces.size();
}

size_t Model::GetFaceVerticesCount(const int findex) const {
	std::vector<uint32_t> face = mFaces[findex];
	//return face.size();
	return mFaces.size() / 3;
}

vec3f Model::GetVertex(const int findex, const int vindex) const {
	std::vector<uint32_t> face = mFaces[findex];
	const int index = face[vindex * 3];
	return mVertices[index];
	//const int index = face[vindex];
	//return mVertices[vindex];
}

vec3f Model::GetNormal(const int findex, const int vindex) const {
	std::vector<uint32_t> face = mFaces[findex];
	const int index = face[(vindex * 3) + 2];
	return mNormals[index];
}

vec2f Model::GetTexcoord0(const int findex, const int vindex) const {
	std::vector<uint32_t> face = mFaces[findex];
	const int index = face[(vindex * 3) + 1];
	return mTexcoord0[index];
}

std::vector<uint32_t> Model::GetFace(const int index) const {
	assert(index >= 0 && index < static_cast<int>(mFaces.size()));
	return mFaces[index];
}

std::shared_ptr<TGAImage> Model::GetDiffuseMap() const {
	return mDiffuseMap;
}

std::shared_ptr<TGAImage> Model::loadTexture(const std::string& filename, const std::string& suffix) {
	std::string texfile(filename);
	size_t dot = texfile.find_last_of(".");
	if (dot != std::string::npos) {
		texfile = texfile.substr(0, dot) + std::string(suffix);
		std::shared_ptr<TGAImage> newImage = std::make_shared<TGAImage>();
		newImage->Read(texfile.c_str());
		newImage->FlipVertically();

		return newImage;
	}

	return nullptr;
}

} // namespace srend