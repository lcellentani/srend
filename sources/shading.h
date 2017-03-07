#pragma once

#include <map>
#include <string>

#include "vector.h"
#include "tgaimage.h"

class ShadingFunction
{
public:
	virtual srend::color_rgba8 operator() () = 0;
};

class ColorFunction : public ShadingFunction
{
public:
	void SetColor(const srend::color_rgba8& color) {
		mColor = color;
	}

	virtual srend::color_rgba8 operator() () override {
		return mColor;
	}

private:
	srend::color_rgba8 mColor;
};

/*class DiffuseShadingFunction : public ShadingFunction
{
public:
	void SetLightDirection(const glm::vec3& lightDirection) {
		mLightDirection = lightDirection;
	}

	void SetFaceNormal(const glm::vec3& normal) {
		mFaceNormal = normal;
	}

	void SetLightColor(const glm::vec4& lightColor) {
		mLightColor = lightColor;
	}

	void SetDiffuseTexture(std::shared_ptr<TGAImage>& texture) {
		mDiffuseMap = texture;
		if (mDiffuseMap) {
			mDiffuseMapWidth = mDiffuseMap->GetWidth();
			mDiffuseMapHeight = mDiffuseMap->GetHeight();
		}
	}

	virtual glm::vec4 operator() (const glm::vec3& pos, const glm::vec3& normal, const glm::vec2& texcoord0) override {
		static const float invColorFloat = 1.0f / 255.0f;
		glm::vec4 color(0.0f, 0.0f, 0.0f, 1.0f);

		float intensity = glm::dot(mFaceNormal, mLightDirection);
		if (intensity >= 0) {
			glm::vec4 diffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
			if (mDiffuseMap) {
				uint32_t px = static_cast<uint16_t>(texcoord0.x * mDiffuseMapWidth);
				uint32_t py = static_cast<uint16_t>(texcoord0.y * mDiffuseMapHeight);
				TGAColor c = mDiffuseMap->GetPixel(px, py);
				diffuseColor.r = c.bgra[2] * invColorFloat;
				diffuseColor.g = c.bgra[1] * invColorFloat;
				diffuseColor.b = c.bgra[0] * invColorFloat;
			}
			color = mLightColor * intensity * diffuseColor;
			color.a = 1.0f;
		}

		return color;
	}

private:
	glm::vec3 mFaceNormal;
	glm::vec3 mLightDirection = { 0.0f, 0.0f, -1.0f };
	glm::vec4 mLightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	std::shared_ptr<TGAImage> mDiffuseMap = nullptr;
	uint16_t mDiffuseMapWidth = 0;
	uint16_t mDiffuseMapHeight = 0;
};*/