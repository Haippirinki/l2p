#pragma once

#include <cstddef>
#include <string>
#include <vector>

std::vector<std::string> getLines(const char* data, size_t size);

std::string format(const char* format, ...);
