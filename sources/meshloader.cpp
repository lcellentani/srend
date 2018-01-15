#include "meshloader.h"
#include "tiny_obj_loader.h"

namespace srend
{

Model::Model() {

}

Model::~Model() {

}

bool Model::IsValid() const {
	return mShapes.size();
}

std::size_t Model::GetNumberOfShapes() const {
	return mShapes.size();
}

const Model::Shape& Model::GetShape(uint32_t index) const {
	return mShapes[index];
}

bool Model::Load(const char* filename, bool triangulate) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	if (tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, nullptr, triangulate)) {
		if (shapes.size()) {
			int32_t numVertices = attrib.vertices.size() / 3;
			int32_t numNormals = attrib.normals.size() / 3;
			int32_t numTexcoords = attrib.texcoords.size() / 2;
			for (std::size_t i = 0; i < shapes.size(); i++) {
				Shape shape;

				size_t index_offset = 0;
				for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
					size_t fnum = shapes[i].mesh.num_face_vertices[f];
					Face face;
					for (size_t v = 0; v < fnum; v++) {
						tinyobj::index_t thisIdx = shapes[i].mesh.indices[index_offset + v];
	
						Point point;
						if (numVertices > 0 && thisIdx.vertex_index < numVertices) {
							point.mPosition.x = attrib.vertices[3 * thisIdx.vertex_index];
							point.mPosition.y = attrib.vertices[3 * thisIdx.vertex_index + 1];
							point.mPosition.z = attrib.vertices[3 * thisIdx.vertex_index + 2];
						}
						if (numNormals > 0 && thisIdx.normal_index < numNormals) {
							point.mNormal.x = attrib.normals[3 * thisIdx.normal_index];
							point.mNormal.y = attrib.normals[3 * thisIdx.normal_index + 1];
							point.mNormal.z = attrib.normals[3 * thisIdx.normal_index + 2];
						}
						if (numTexcoords > 0 && thisIdx.texcoord_index < numTexcoords) {
							point.mTexcoord.x = attrib.texcoords[2 * thisIdx.texcoord_index];
							point.mTexcoord.y = attrib.texcoords[2 * thisIdx.texcoord_index + 1];
						}

						face.mVertices.push_back(point);
					}
					shape.mFaces.push_back(face);

					index_offset += fnum;
				}
				mShapes.push_back(shape);
			}
			return true;
		}
	}
	return false;
}

} // namespace srend