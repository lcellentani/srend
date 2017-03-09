#include <cmath>
#include "tgaimage.h"
#include "model_obj.h"
#include "rasterizer.h"
#include "shading.h"

#include <memory>
#include <iostream>
#include <limits>

const int depth = 255;

/*glm::mat4 viewport(float x, float y, float w, float h) {
	glm::mat4 m(1.0f);
	m[0][3] = x + w / 2.f;
	m[1][3] = y + h / 2.f;
	m[2][3] = depth / 2.f;

	m[0][0] = w / 2.f;
	m[1][1] = h / 2.f;
	m[2][2] = depth / 2.f;

	return m;
}*/

/*
glm::vec3 world2screen(const glm::vec3 v, float width, float height) {
	return glm::vec3(int((v.x + 1.0f)*width + 0.5f), int((v.y + 1.0f)*height + 0.5f), v.z);
}

void renderDiffuseModelOrthoMode(const std::shared_ptr<Model>& model, srend::Rasterizer& rasterizer)
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
	diffuseFunction.SetDiffuseTexture(model->GetDiffuseMap());

	for (size_t i = 0; i < model->NumberOfFaces(); i++) {
		for (int j = 0; j < 3; j++) {
			world_coords[j] = model->GetVertex(i, j);

			vertices[j].pos = world2screen(world_coords[j], halfWidth, halfHeight);
			vertices[j].uv0 = model->GetTexcoord0(i, j);
		}

		glm::vec3 v1 = world_coords[2] - world_coords[0];
		glm::vec3 v2 = world_coords[1] - world_coords[0];
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);
		diffuseFunction.SetFaceNormal(normal);

		rasterizer.DrawTriangle(vertices[0], vertices[1], vertices[2], diffuseFunction);
	}

}

void renderDiffuseModelPerspectiveMode(const std::shared_ptr<Model>& model, srend::Rasterizer& rasterizer)
{
	const float width = rasterizer.GetFramebufferWidth();
	const float height = rasterizer.GetFramebufferHeight();
	const float halfWidth = width * 0.5f;
	const float halfHeight = height * 0.5f;

	glm::vec3 cameraPosition(0.0f, 0.0f, 3.0f);

	glm::mat4 Projection(1.0f);
	glm::mat4 View = viewport(width / 8.0f, height / 8.0f, width * 3.0f / 4.0f, height * 3.0f / 4.0f);
	Projection[3][2] = -1.f / cameraPosition.z;

	const glm::vec4 lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	const glm::vec3 lightDirection = { 0.0f, 0.0f, -1.0f };

	std::vector<srend::Vertex> vertices;
	vertices.reserve(3);
	std::vector<glm::vec3> world_coords;
	world_coords.reserve(3);

	DiffuseShadingFunction diffuseFunction;
	diffuseFunction.SetLightDirection(lightDirection);
	diffuseFunction.SetLightColor(lightColor);
	diffuseFunction.SetDiffuseTexture(model->GetDiffuseMap());

	for (size_t i = 0; i < model->NumberOfFaces(); i++) {
		for (int j = 0; j < 3; j++) {
			glm::vec3 v = model->GetVertex(i, j);

			glm::vec4 tmp(v.x, v.y, v.z, 1.0f);
			tmp = tmp * View * Projection;

			vertices[j].pos = glm::vec3(tmp.x, tmp.y, tmp.z);
			vertices[j].uv0 = model->GetTexcoord0(i, j);

			world_coords[j] = vertices[j].pos;
		}

		glm::vec3 v1 = world_coords[2] - world_coords[0];
		glm::vec3 v2 = world_coords[1] - world_coords[0];
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);
		diffuseFunction.SetFaceNormal(normal);

		rasterizer.DrawTriangle(vertices[0], vertices[1], vertices[2], diffuseFunction);
	}

}
*/

#define TEST 4

int main(int argc, char **argv)
{
	srend::Rasterizer rasterizer;

#if TEST == 0
	{
		const int width = 200;
		const int height = 200;
		rasterizer.SetFramebufferSize(width, height);
		rasterizer.SetTexel(srend::vec2i(100, 100), srend::color_rgba8(255, 0, 0, 255));
		rasterizer.SetTexel(srend::vec2i(0, 0), srend::color_rgba8(0, 0, 255, 255));
	}
#elif TEST == 1
	{
		std::shared_ptr<srend::Model> model = std::make_shared<srend::Model>();
		model->Load("african_head/african_head.obj");

		srend::color_rgba8 cWhite(255, 255, 255, 255);
		const int width = 800;
		const int height = 800;
		const float halfWidth = width * 0.5f;
		const float halfHeight = height * 0.5f;

		rasterizer.SetFramebufferSize(width, height);

		for (size_t i = 0; i < model->NumberOfFaces(); i++) {
			for (int j = 0; j < 3; j++) {
				srend::vec3f v0 = model->GetVertex(i, j);
				srend::vec3f v1 = model->GetVertex(i, (j + 1) % 3);

				int32_t x0 = static_cast<int32_t>((v0.x() + 1.0f) * halfWidth);
				int32_t y0 = static_cast<int32_t>((v0.y() + 1.0f) * halfHeight);
				int32_t x1 = static_cast<int32_t>((v1.x() + 1.0f) * halfWidth);
				int32_t y1 = static_cast<int32_t>((v1.y() + 1.0f) * halfHeight);

				srend::vec2f p0(x0, y0);
				srend::vec2f p1(x1, y1);
				rasterizer.DrawLine(p0, p1, cWhite);
			}
		}
	}
#elif TEST == 2
	{
		const int width = 200;
		const int height = 200;
		rasterizer.SetFramebufferSize(width, height);

		srend::color_rgba8 cRed(255, 0, 0, 255);
		srend::color_rgba8 cWhite(255, 255, 255, 255);
		srend::color_rgba8 cGreen(0, 255, 0, 255);
		srend::color_rgba8 cMagenta(255, 0, 255, 255);

		ColorFunction colorOperator;

		srend::vec2f t0[3] = { srend::vec2f(10, 70), srend::vec2f(50, 160), srend::vec2f(70, 80) };
		srend::vec2f t1[3] = { srend::vec2f(180, 50), srend::vec2f(150, 1), srend::vec2f(70, 180) };
		srend::vec2f t2[3] = { srend::vec2f(180, 150), srend::vec2f(120, 160), srend::vec2f(130, 180) };


		colorOperator.SetColor(cRed);
		rasterizer.DrawTriangle(t0[0], t0[1], t0[2], colorOperator);
		//rasterizer.DrawLine(t0[0], t0[1], cMagenta);
		//rasterizer.DrawLine(t0[1], t0[2], cMagenta);
		//rasterizer.DrawLine(t0[2], t0[0], cMagenta);
		colorOperator.SetColor(cWhite);
		rasterizer.DrawTriangle(t1[0], t1[1], t1[2], colorOperator);
		//rasterizer.DrawLine(t1[0], t1[1], cMagenta);
		//rasterizer.DrawLine(t1[1], t1[2], cMagenta);
		//rasterizer.DrawLine(t1[2], t1[0], cMagenta);
		colorOperator.SetColor(cGreen);
		rasterizer.DrawTriangle(t2[0], t2[1], t2[2], colorOperator);
		//rasterizer.DrawLine(t2[0], t2[1], cMagenta);
		//rasterizer.DrawLine(t2[1], t2[2], cMagenta);
		//rasterizer.DrawLine(t2[2], t2[0], cMagenta);
	}
#elif TEST == 3
	{
		std::shared_ptr<srend::Model> model = std::make_shared<srend::Model>();
		model->Load("african_head/african_head.obj");

		srend::color_rgba8 cWhite(255, 255, 255, 255);
		const int width = 800;
		const int height = 800;
		const float halfWidth = width * 0.5f;
		const float halfHeight = height * 0.5f;

		rasterizer.SetFramebufferSize(width, height);

		ColorFunction colorOperator;
		srend::vec3f lightDir(0.0f, 0.0f, -1.0f);
		srend::vec2f screen_coords[3];
		srend::vec3f world_coords[3];
		for (size_t i = 0; i < model->NumberOfFaces(); i++) {
			for (int j = 0; j < 3; j++) {
				srend::vec3f v = model->GetVertex(i, j);

				screen_coords[j] = srend::vec2f(int((v.x() + 1.0f) * halfWidth + 0.5f), int((v.y() + 1.0f) * halfHeight + 0.5f));
				world_coords[j] = v;
			}

			srend::vec3f n = srend::vec3f::CrossProduct((world_coords[2] - world_coords[0]), (world_coords[1] - world_coords[0]));
			n.Normalize();
			float intensity = srend::vec3f::DotProduct(n, lightDir);
			if (intensity > 0) {
				uint8_t r = static_cast<uint8_t>(std::roundf((intensity * 255.0f)));
				uint8_t g = static_cast<uint8_t>(std::roundf((intensity * 255.0f)));
				uint8_t b = static_cast<uint8_t>(std::roundf((intensity * 255.0f)));
				srend::color_rgba8 color(r, g, b);
				colorOperator.SetColor(color);

				rasterizer.DrawTriangle(screen_coords[0], screen_coords[1], screen_coords[2], colorOperator);
			}
		}
	}
#elif TEST == 4
	{
		std::shared_ptr<srend::Model> model = std::make_shared<srend::Model>();
		model->Load("african_head/african_head.obj");

		srend::color_rgba8 cWhite(255, 255, 255, 255);
		const int width = 800;
		const int height = 800;
		const float halfWidth = width * 0.5f;
		const float halfHeight = height * 0.5f;

		rasterizer.SetFramebufferSize(width, height);

		ColorFunction colorOperator;
		srend::vec3f lightDir(0.0f, 0.0f, -1.0f);
		std::vector<srend::vec3f> screen_coords{ srend::vec3f(), srend::vec3f(), srend::vec3f() };
		srend::vec3f world_coords[3];
		for (size_t i = 0; i < model->NumberOfFaces(); i++) {
			for (int j = 0; j < 3; j++) {
				srend::vec3f v = model->GetVertex(i, j);

				screen_coords[j] = srend::vec3f(int((v.x() + 1.0f) * halfWidth + 0.5f), int((v.y() + 1.0f) * halfHeight + 0.5f), v.z());
				world_coords[j] = v;
			}

			srend::vec3f n = srend::vec3f::CrossProduct((world_coords[2] - world_coords[0]), (world_coords[1] - world_coords[0]));
			n.Normalize();
			float intensity = srend::vec3f::DotProduct(n, lightDir);
			if (intensity > 0) {
				uint8_t r = static_cast<uint8_t>(std::roundf((intensity * 255.0f)));
				uint8_t g = static_cast<uint8_t>(std::roundf((intensity * 255.0f)));
				uint8_t b = static_cast<uint8_t>(std::roundf((intensity * 255.0f)));
				srend::color_rgba8 color(r, g, b);
				colorOperator.SetColor(color);

				rasterizer.DrawTriangle(screen_coords, colorOperator);
			}
		}
	}
#elif TEST == 5
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
		for (int i = 0; i < width; i++) {
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
#elif TEST == 6
	const int width = 100;
	const int height = 100;
	rasterizer.SetFramebufferSize(width, height);

	std::shared_ptr<Model> model = std::make_shared<Model>();
	model->Load("cube.obj");

	glm::mat4 VP = viewport(width / 4, width / 4, width / 2, height / 2);

	// draw the axes
	glm::vec4 x(1.f, 0.f, 0.f, 1.0f);
	glm::vec4 y(0.f, 1.f, 0.f, 1.0f);
	glm::vec4 o(0.f, 0.f, 0.f, 1.0f);

	o = o * VP;
	x = x * VP;
	y = y * VP;

	rasterizer.DrawLine(glm::vec2(o.x, o.y), glm::vec2(x.x, x.y), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	rasterizer.DrawLine(glm::vec2(o.x, o.y), glm::vec2(y.x, y.y), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	size_t num = model->NumberOfFaces();
	for (size_t i = 0; i < num; i++) {
		int faceSize = model->GetFaceVerticesCount(i);
		for (int j = 0; j < faceSize; j++) {
			glm::vec3 v0 = model->GetVertex(i, j);
			glm::vec3 v1 = model->GetVertex(i, (j + 1) % faceSize);

			// draw the original model
			glm::vec4 p0(v0.x, v0.y, v0.z, 1.0f);
			glm::vec4 p1(v1.x, v1.y, v1.z, 1.0f);

			p0 = p0 * VP;
			p1 = p1 * VP;

			rasterizer.DrawLine(glm::vec2(p0.x, p0.y), glm::vec2(p1.x, p1.y), glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));

			// draw the deformed model
			glm::mat4 T(1.0f);
			T[0][0] = 1.5f;
			T[1][1] = 1.5f;
			T[2][2] = 1.5f;

			// draw the original model
			glm::vec4 p2(v0.x, v0.y, v0.z, 1.0f);
			glm::vec4 p3(v1.x, v1.y, v1.z, 1.0f);

			p2 = p2 * T * VP;
			p3 = p3 * T * VP;

			rasterizer.DrawLine(glm::vec2(p2.x, p2.y), glm::vec2(p3.x, p3.y), glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));
		}
		break;
	}
#elif TEST == 7
	std::shared_ptr<Model> model = std::make_shared<Model>();
	model->Load("african_head/african_head.obj");

	const int width = 800;
	const int height = 800;
	rasterizer.SetFramebufferSize(width, height);
	renderDiffuseModelPerspectiveMode(model, rasterizer);
#else
	assert(0);
#endif
	rasterizer.Save("output.tga");

	return 0;
}
