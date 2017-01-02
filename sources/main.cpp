#include <cmath>
#include "tgaimage.h"
#include "model_obj.h"
#include "rasterizer.h"

#include <glm/glm.hpp>

#include <memory>

const TGAColor cWHITE = TGAColor(255, 255, 255, 255);
const TGAColor cRED = TGAColor(255, 0, 0, 255);
const TGAColor cGREEN = TGAColor(0, 255, 0, 255);
const TGAColor cYELLOW = TGAColor(255, 255, 0, 255);
const TGAColor cMANGENTA = TGAColor(255, 0, 255, 255);

glm::vec3 barycentric(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, glm::vec2& P) {
	glm::vec3 v1(p2.x - p0.x, p1.x - p0.x, p0.x - P.x);
	glm::vec3 v2(p2.y - p0.y, p1.y - p0.y, p0.y - P.y);
	
	glm::vec3 u = glm::cross(v1, v2);// cross(Vec3f(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - P[0]), Vec3f(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - P[1]));
	if (std::abs(u[2]) < 1) return glm::vec3(-1.0f, 1.0f, 1.0f); // triangle is degenerate, in this case return smth with negative coordinates 
	return glm::vec3(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void bresenham(int x0, int y0, int x1, int y1, const TGAColor& color, TGAImage& image)
{
#if _DEBUG
	image.SetPixel(x0, y0, cMANGENTA);
	image.SetPixel(x1, y1, cMANGENTA);
#endif

	bool steep = std::abs(x0 - x1) < std::abs(y0 - y1);
	if (steep) {
		std::swap(x0, y0);
		std::swap(x1, y1);
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = std::abs(y1 - y0);
	int error = dx >> 1;
	int ystep = (y1 > y0 ? 1 : -1);
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		image.SetPixel(steep ? y : x, steep ? x : y, color);
		error -= dy;
		if (error < 0) {
			y += ystep;
			error += dx;
		}
	}
}

void drawLine(const glm::vec2& p0, const glm::vec2& p1, const TGAColor& color, TGAImage& image) {
	glm::vec2 a(p0);
	glm::vec2 b(p1);
	
	bool steep = std::abs(a.x - b.x) < std::abs(a.y - b.y);
	if (steep) {
		std::swap(a.x, a.y);
		std::swap(b.x, b.y);
	}
	if (a.x > b.x) {
		std::swap(a.x, b.x);
		std::swap(a.y, b.y);
	}

	float dx = b.x - a.x;
	float dy = b.y - a.y;
	float gradient = dy / dx;

	int xx, yy;

	xx = static_cast<int>(steep ? a.y : a.x);
	yy = static_cast<int>(steep ? a.x : a.y);
	image.SetPixel(xx, yy, color);

	xx = static_cast<int>(steep ? b.y : b.x);
	yy = static_cast<int>(steep ? b.x : b.y);
	image.SetPixel(xx, yy, color);

	float y = a.y + gradient;
	float x0 = a.x;
	float x1 = b.x;
	for (float x = x0 + 1; x <= x1 - 1; x++) {
		float a2 = y - ((int)y);
		float a1 = 1.0f - a2;
		TGAColor c1, c2;
		c1 = color * a1;
		c2 = color * a2;
		
		xx = static_cast<int>(std::floor(x));
		yy = static_cast<int>(std::floor(y));
		int yy1 = yy + 1;
		image.SetPixel(steep ? yy : xx, steep ? xx : yy, c1);
		image.SetPixel(steep ? yy1 : xx, steep ? xx : yy1, c2);

		y += gradient;
	}
}

void triangle(glm::vec2& t0, glm::vec2& t1, glm::vec2& t2, const TGAColor& color, TGAImage& image) {
	if (t0.y == t1.y && t0.y == t2.y) return;

	if (t0.y > t1.y) std::swap(t0, t1);
	if (t0.y > t2.y) std::swap(t0, t2);
	if (t1.y > t2.y) std::swap(t1, t2);

	//drawLine(t0, t1, color, image);
	//drawLine(t1, t2, color, image);
	//drawLine(t2, t0, color, image);

	/*int total_height = static_cast<int>(t2.y - t0.y);
	for (int i = 0; i < total_height; i++) {
		bool second_half = (i > (t1.y - t0.y)) || (t1.y == t0.y);
		int segment_height = second_half ? (t2.y - t1.y) : (t1.y - t0.y);
		float alpha = (float)i / total_height;
		float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height; // be careful: with above conditions no division by zero here 
		glm::vec2 A = t0 + (t2 - t0)*alpha;
		glm::vec2 B = second_half ? t1 + (t2 - t1)*beta : t0 + (t1 - t0)*beta;
		if (A.x>B.x) std::swap(A, B);
		for (int j = A.x; j <= B.x; j++) {
			image.set(j, t0.y + i, color); // attention, due to int casts t0.y+i != A.y 
		}
	}*/

}

int main(int argc, char **argv)
{
	//std::shared_ptr<Model> model = std::make_shared<Model>();
	//model->Load("african_head/african_head.obj");

	//int width = 500;
	//int height = 500;
	//
	//float halfWidth = static_cast<float>(width) * 0.5f;
	//float halfHeight = static_cast<float>(height) * 0.5f;

	srend::Rasterizer rasterizer;
	rasterizer.SetFramebufferSize(200, 200);
	rasterizer.SetTexel(3, 3);
	rasterizer.Save("output.tga");

	/*TGAImage image(width, height, TGAImage::RGB);
	
	for (size_t i = 0; i < model->NumberOfFaces(); i++) {
		std::vector<int> face = model->GetFace(i);
		for (int j = 0; j < 3; j++) {
			glm::vec3 v0 = model->GetVertex(face[j]);
			glm::vec3 v1 = model->GetVertex(face[(j + 1) % 3]);

			float x0 = (v0.x + 1.0f) * halfWidth;
			float y0 = (v0.y + 1.0f) * halfHeight;
			float x1 = (v1.x + 1.0f) * halfWidth;
			float y1 = (v1.y + 1.0f) * halfHeight;
			drawLine(glm::vec2(x0, y0), glm::vec2(x1, y1), cWHITE, image);
			//bresenham(x0, y0, x1, y1, cWHITE, image);
		}
	}

	//image.set(0, 0, cMANGENTA);

	//triangle(glm::vec2(10.0f, 70.0f), glm::vec2(50.0f, 160.0f), glm::vec2(70.0f, 80.0f), cRED, image);
	//triangle(glm::vec2(180.0f, 50.0f), glm::vec2(150.0f, 1.0f), glm::vec2(70.0f, 180.0f), cWHITE, image);
	//triangle(glm::vec2(180.0f, 150.0f), glm::vec2(120.0f, 160.0f), glm::vec2(130.0f, 180.0f), cGREEN, image);

	image.FlipVertically();
	image.Write("output.tga");*/

	return 0;
}
