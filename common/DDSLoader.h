#pragma once

#include <cstddef>

typedef unsigned int GLenum;
typedef unsigned int GLuint;

extern GLuint loadDDS(const void* data, size_t size, bool sRGB, size_t& width, size_t& height, size_t& depth, GLenum& bindTarget);
