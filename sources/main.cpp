#include <cmath>
#include "tgaimage.h"
#include "model_obj.h"

#include <glm/vec3.hpp>

#include <memory>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor yellow = TGAColor(255, 255, 0, 255);
const TGAColor magenta = TGAColor(255, 0, 255, 255);

void line(int x0, int y0, int x1, int y1, TGAImage& image, const TGAColor& color)
{
#if _DEBUG
	image.set(x0, y0, magenta);
	image.set(x1, y1, magenta);
#endif

#if 0
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
		image.set(steep ? y : x, steep ? x : y, color);
		error -= dy;
		if (error < 0) {
			y += ystep;
			error += dx;
		}
	}
#else
	bool steep = std::abs(x0 - x1) < std::abs(y0 - y1);
	if (steep) {
		std::swap(x0, y0);
		std::swap(x1, y1);
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	float dx = static_cast<float>(x1 - x0);
	float dy = static_cast<float>(y1 - y0);
	float gradient = dy / dx;

	image.set(steep ? y0 : x0, steep ? x0 : y0, color);
	image.set(steep ? y1 : x1, steep ? x1 : y1, color);

	float y = y0 + gradient;
	for (int x = x0 + 1; x <= x1 - 1; x++) {
		float a2 = y - ((int)y);
		float a1 = 1.0f - a2;
		TGAColor c1, c2;
		c1 = color * a1;
		c2 = color * a2;
		int xx = static_cast<int>(std::floor(x));
		int yy = static_cast<int>(std::floor(y));
		int yy1 = yy + 1;
		image.set(steep ? yy : xx, steep ? xx : yy, c1);
		image.set(steep ? yy1 : xx, steep ? xx : yy1, c2);

		y += gradient;
	}
#endif
}

int main(int argc, char **argv)
{
	std::shared_ptr<Model> model = std::make_shared<Model>();
	model->Load("african_head/african_head.obj");

	int width = 800;
	int height = 800;

	float halfWidth = static_cast<float>(width) * 0.5f;
	float halfHeight = static_cast<float>(height) * 0.5f;

	TGAImage image(width, height, TGAImage::RGB);
	
	for (size_t i = 0; i < model->NumberOfFaces(); i++) {
		std::vector<int> face = model->GetFace(i);
		for (int j = 0; j < 3; j++) {
			glm::vec3 v0 = model->GetVertex(face[j]);
			glm::vec3 v1 = model->GetVertex(face[(j + 1) % 3]);

			int x0 = static_cast<int>((v0.x + 1.0f) * halfWidth);
			int y0 = static_cast<int>((v0.y + 1.0f) * halfHeight);
			int x1 = static_cast<int>((v1.x + 1.0f) * halfWidth);
			int y1 = static_cast<int>((v1.y + 1.0f) * halfHeight);
			line(x0, y0, x1, y1, image, white);
		}
	}

	image.flip_vertically();
	image.write_tga_file("output.tga");

	return 0;
}
