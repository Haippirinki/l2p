#pragma once

#include <cstddef>

typedef unsigned int GLenum;
typedef unsigned int GLuint;

extern GLuint loadKTX(const void* data, size_t size, size_t& width, size_t& height, size_t& depth, GLenum& bindTarget);
