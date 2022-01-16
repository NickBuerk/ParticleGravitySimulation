#pragma once

#include <fstream>
#include <functional>

namespace pgs
{

// from: https://stackoverflow.com/a/57595105
template <typename T, typename... Rest>
void hashCombine(std::size_t &seed, const T &v, const Rest &...rest)
{
	seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	(hashCombine(seed, rest), ...);
};

static std::vector<char> readShaderFile(const std::string &filepath)
{
	std::ifstream file{filepath, std::ios::ate | std::ios::binary};

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file: " + filepath);
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}

} // namespace pgs
