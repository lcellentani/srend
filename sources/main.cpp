#include <cmath>
#include "tgaimage.h"
#include "model_obj.h"
#include "rasterizer.h"
#include "shading.h"

#include "glm/glm.hpp"

#include <memory>
#include <iostream>
#include <limits>

/*void triangle(glm::vec2& t0, glm::vec2& t1, glm::vec2& t2, const TGAColor& color, TGAImage& image) {
	if (t0.y == t1.y && t0.y == t2.y) return;

	if (t0.y > t1.y) std::swap(t0, t1);
	if (t0.y > t2.y) std::swap(t0, t2);
	if (t1.y > t2.y) std::swap(t1, t2);

	//drawLine(t0, t1, color, image);
	//drawLine(t1, t2, color, image);
	//drawLine(t2, t0, color, image);

	//int total_height = static_cast<int>(t2.y - t0.y);
	//for (int i = 0; i < total_height; i++) {
	//	bool second_half = (i > (t1.y - t0.y)) || (t1.y == t0.y);
	//	int segment_height = second_half ? (t2.y - t1.y) : (t1.y - t0.y);
	//	float alpha = (float)i / total_height;
	//	float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height; // be careful: with above conditions no division by zero here 
	//	glm::vec2 A = t0 + (t2 - t0)*alpha;
	//	glm::vec2 B = second_half ? t1 + (t2 - t1)*beta : t0 + (t1 - t0)*beta;
	//	if (A.x>B.x) std::swap(A, B);
	//	for (int j = A.x; j <= B.x; j++) {
	//		image.set(j, t0.y + i, color); // attention, due to int casts t0.y+i != A.y 
	//	}
	//}

}
*/

void renderWireframeModel(const std::shared_ptr<Model>& model, srend::Rasterizer& rasterizer)
{
	const float halfWidth = rasterizer.GetFramebufferWidth() * 0.5f;
	const float halfHeight = rasterizer.GetFramebufferHeight() * 0.5f;

	for (size_t i = 0; i < model->NumberOfFaces(); i++) {
		std::vector<int> face = model->GetFace(i);
		for (int j = 0; j < 3; j++) {
			glm::vec3 v0 = model->GetVertex(face[j]);
			glm::vec3 v1 = model->GetVertex(face[(j + 1) % 3]);

			glm::vec2 p0((v0.x + 1.0f) * halfWidth, (v0.y + 1.0f) * halfHeight);
			glm::vec2 p1((v1.x + 1.0f) * halfWidth, (v1.y + 1.0f) * halfHeight);
			rasterizer.DrawLine(p0, p1, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}
}

glm::vec3 world2screen(const glm::vec3 v, float width, float height) {
	return glm::vec3(int((v.x + 1.0f)*width + 0.5f), int((v.y + 1.0f)*height + 0.5f), v.z);
}

void renderDiffuseModel(const std::shared_ptr<Model>& model, srend::Rasterizer& rasterizer)
{
	const float halfWidth = rasterizer.GetFramebufferWidth() * 0.5f;
	const float halfHeight = rasterizer.GetFramebufferHeight() * 0.5f;

	const glm::vec4 lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	const glm::vec3 lightDirection = { 0.0f, 0.0f, -1.0f };

	std::vector<srend::Vertex> vertices;
	vertices.reserve(3);
	std::vector<glm::vec3> world_coords;
	world_coords.reserve(3);
	
	DiffuseShadingFunction diffuseFunction;
	diffuseFunction.SetLightDirection(lightDirection);
	diffuseFunction.SetLightColor(lightColor);
	diffuseFunction.SetTexture(std::string("diffuseMap"), model->GetDiffuseMap());

	for (size_t i = 0; i < model->NumberOfFaces(); i++) {
		std::vector<int> face = model->GetFace(i);
		for (int j = 0; j < 3; j++) {
			world_coords[j] = model->GetVertex(face[j]);

			//glm::vec3 screen_coord((world_coords[j].x + 1.0f) * halfWidth + 0.5f, (world_coords[j].y + 1.0f) * halfHeight + 0.5f, world_coords[j].z);
			//screen_coords[j] = screen_coord;
			vertices[j].pos = world2screen(world_coords[j], halfWidth, halfHeight);
		}

		glm::vec3 v1 = world_coords[2] - world_coords[0];
		glm::vec3 v2 = world_coords[1] - world_coords[0];
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		vertices[0].norm = normal;
		vertices[1].norm = normal;
		vertices[2].norm = normal;

		rasterizer.DrawTriangle(vertices[0], vertices[1], vertices[2], diffuseFunction);
	}

}

void rasterize(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, int ybuffer[], srend::Rasterizer& rasterizer)
{
	glm::vec2 a(p0);
	glm::vec2 b(p1);

	if (a.x > b.x) {
		std::swap(a, b);
	}

	float dx = b.x - a.x;
	for (int x = a.x; x <= b.x; x++) {
		float t = (x - a.x) / dx;
		int y = a.y * (1.0f - t) + b.y * t + 0.5f;
		if (ybuffer[x]<y) {
			ybuffer[x] = y;
			rasterizer.SetTexel(x, 0, color);
		}
	}
}

#define TEST 3

int main(int argc, char **argv)
{
	std::shared_ptr<Model> model = std::make_shared<Model>();
	model->Load("african_head/african_head.obj");

	srend::Rasterizer rasterizer;

#if TEST == 0
	{
		const int width = 200;
		const int height = 200;
		rasterizer.SetFramebufferSize(width, height);
		rasterizer.SetTexel(50, 50, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
#elif TEST == 1
	{
		const int width = 800;
		const int height = 800;
		rasterizer.SetFramebufferSize(width, height);
		renderWireframeModel(model, rasterizer);
	}
#elif TEST == 2
	{
		const int width = 200;
		const int height = 200;
		rasterizer.SetFramebufferSize(width, height);
		rasterizer.DrawTriangle(glm::vec2(10.0f, 70.0f), glm::vec2(50.0f, 160.0f), glm::vec2(70.0f, 80.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		rasterizer.DrawTriangle(glm::vec2(180.0f, 50.0f), glm::vec2(150.0f, 1.0f), glm::vec2(70.0f, 180.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		rasterizer.DrawTriangle(glm::vec2(180.0f, 150.0f), glm::vec2(120.0f, 160.0f), glm::vec2(130.0f, 180.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	}
#elif TEST == 3
	{
		const int width = 800;
		const int height = 800;
		rasterizer.SetFramebufferSize(width, height);
		renderDiffuseModel(model, rasterizer);
	}
#elif TEST == 4
	{
		const int width = 800;
		const int height = 16;
		rasterizer.SetFramebufferSize(width, height);
		// scene "2d mesh"
		rasterizer.DrawLine(glm::vec2(20, 34), glm::vec2(744, 400), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		rasterizer.DrawLine(glm::vec2(120, 434), glm::vec2(444, 400), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		rasterizer.DrawLine(glm::vec2(330, 463), glm::vec2(594, 200), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

		// screen line
		rasterizer.DrawLine(glm::vec2(10, 10), glm::vec2(790, 10), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		int ybuffer[width];
		for (int i = 0; i<width; i++) {
			ybuffer[i] = std::numeric_limits<int>::min();
		}
		
		rasterize(glm::vec2(20, 34), glm::vec2(744, 400), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), ybuffer, rasterizer);
		rasterize(glm::vec2(120, 434), glm::vec2(444, 400), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), ybuffer, rasterizer);
		rasterize(glm::vec2(330, 463), glm::vec2(594, 200), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), ybuffer, rasterizer);

		// 1-pixel wide image is bad for eyes, lets widen it
		for (int i = 0; i < width; i++) {
			for (int j = 1; j < 16; j++) {
				glm::vec4 c = rasterizer.GetTexel(i, 0);
				rasterizer.SetTexel(i, j, c);
			}
		}
	}
#else
	assert(0);
#endif
	rasterizer.Save("output.tga");

	return 0;
}
