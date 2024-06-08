#pragma once

#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"

namespace MyGL
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 tex_coords;
	};

	class Mesh
	{
	public:
		Mesh(const std::vector<Vertex> &vertices,
			 const std::vector<GLuint> &indices);

		Mesh(const std::vector<glm::vec3> &vertices,
			 const std::vector<GLuint> &indices);

		~Mesh();

		void draw();
		void draw_wireframe();

	private:
		GLuint VAO, VBO, EBO;

		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;

		void setup();
	};
}