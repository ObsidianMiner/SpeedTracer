#pragma once

#include <glm/glm.hpp>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "shaderClass.h"

class Text
{
public:
	static int LoadFont();
	static void RenderText(Shader& s, std::string text, float x, float y, float scale, int alignment, glm::vec3 color);
};
#define LEFT_ALIGN 0
#define MIDDLE_ALIGN 1
#define RIGHT_ALIGN 2