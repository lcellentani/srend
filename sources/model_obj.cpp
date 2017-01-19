#include "model_obj.h"

#include <fstream>
#include <string>
#include <sstream>
#include <cassert>

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
					glm::vec3 vertex;

					iss >> discard_v;

					iss >> vertex.x;
					iss >> vertex.y;
					iss >> vertex.z;

					mVertices.push_back(vertex);

					//iss >> trash;
					//Vec3f v;
					//for (int i = 0; i<3; i++) iss >> v[i];
					//verts_.push_back(v);

				}
				else if (line.compare(0, 3, "vn ") == 0) {


					//iss >> trash >> trash;
					//Vec3f n;
					//for (int i = 0; i<3; i++) iss >> n[i];
					//norms_.push_back(n);
				}
				else if (line.compare(0, 3, "vt ") == 0) {


					//iss >> trash >> trash;
					//Vec2f uv;
					//for (int i = 0; i<2; i++) iss >> uv[i];
					//uv_.push_back(uv);
				}
				else if (line.compare(0, 2, "f ") == 0) {
					char discard_f;
					std::vector<int> indices;

					iss >> discard_f;

					char discard;
					int vtxIndex, texIndex, normIndex;
					while (iss >> vtxIndex >> discard >> texIndex >> discard >> normIndex) {
						vtxIndex--; // in wavefront obj all indices start at 1, not zero
						indices.push_back(vtxIndex);
					}
					mFaces.push_back(indices);

					//std::vector<Vec3i> f;
					//Vec3i tmp;
					//iss >> trash;
					//while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
					//	for (int i = 0; i<3; i++) tmp[i]--; // in wavefront obj all indices start at 1, not zero
					//	f.push_back(tmp);
					//}
					//faces_.push_back(f);
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

glm::vec3 Model::GetVertex(const int index) const {
	assert(index >= 0 && index < static_cast<int>(mVertices.size()));
	return mVertices[index];
}

glm::vec3 Model::GetNormal(const int index) const {
	assert(index >= 0 && index < static_cast<int>(mNormals.size()));
	return mNormals[index];
}

glm::vec2 Model::GetTexcoord0(const int index) const {
	assert(index >= 0 && index < static_cast<int>(mTexcoord0.size()));
	return mTexcoord0[index];
}

std::vector<int> Model::GetFace(const int index) const {
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