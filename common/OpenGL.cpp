#include "OpenGL.h"

#ifdef GL_ES_VERSION_3_0
const char* glslVersion = "#version 300 es\n";
#else
const char* glslVersion = "#version 330 core\n";
#endif
