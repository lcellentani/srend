#include "model_obj.h"

#include <fstream>
#include <string>
#include <sstream>
#include <cassert>

Model::Model() : mVertices(), mFaces() {

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
				}
			}
		}
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

std::vector<int> Model::GetFace(const int index) const {
	assert(index >= 0 && index < static_cast<int>(mFaces.size()));
	return mFaces[index];
}