#pragma once

#include <map>
#include <string>

#include "glm/glm.hpp"
#include "tgaimage.h"

class ShadingFunction
{
public:
	virtual glm::vec4 operator() (const glm::vec3& pos, const glm::vec3& normal, const glm::vec2& texcoord0) = 0;

	void SetTexture(const std::string& name, std::shared_ptr<TGAImage>& texture) {
		mTextures.insert(std::make_pair(name, texture));
	}

private:
	std::map<std::string, std::shared_ptr<TGAImage>> mTextures;
};

class DiffuseShadingFunction : public ShadingFunction
{
public:
	void SetLightDirection(const glm::vec3& lightDirection) {
		mLightDirection = lightDirection;
	}

	void SetLightColor(const glm::vec4& lightColor) {
		mLightColor = lightColor;
	}

	virtual glm::vec4 operator() (const glm::vec3& pos, const glm::vec3& normal, const glm::vec2& texcoord0) override {
		glm::vec4 color(0.0f, 0.0f, 0.0f, 1.0f);

		float intensity = glm::dot(normal, mLightDirection);
		if (intensity >= 0) {
			color = mLightColor * intensity;
			color.a = 1.0f;
		}

		return color;
	}

private:
	glm::vec3 mLightDirection = { 0.0f, 0.0f, -1.0f };
	glm::vec4 mLightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
};