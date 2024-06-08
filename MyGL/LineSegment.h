#pragma once

#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"

namespace MyGL
{
	class LineSegment
	{
	public:
		LineSegment(const std::vector<glm::vec3> &vertices,
					const std::vector<GLuint> &indices);

		~LineSegment();

		void draw();
		void draw_wireframe();

	private:
		GLuint VAO, VBO, EBO;

		std::vector<glm::vec3> vertices;
		std::vector<GLuint> indices;

		void setup();
	};
}