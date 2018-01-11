#pragma once

#include <cstdint>
#include <functional>
#include <vector>

class Display final {
public:
	Display(uint32_t width, uint32_t height, const char* caption);
	~Display();

	bool IsValid() const;

	void Present();

	void SetFramebufferUpdater(std::function<void(uint32_t width, uint32_t height, uint8_t depth, std::vector<uint8_t>& brga)> func);

private:
	struct Impl;
	Impl* mImpl;
};