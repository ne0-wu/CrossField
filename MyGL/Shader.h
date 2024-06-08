#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace MyGL
{
	class Shader
	{
	public:
		Shader(GLenum shader_type);
		~Shader();

		GLuint get_ID() const { return ID; }

		void compile_from_source(const std::string &source);
		void compile_from_file(const std::string &file_path);

	private:
		GLuint ID;
		GLenum type;

		static std::string read_file_to_string(const std::string &file_path);
	};

	class ShaderProgram
	{
	public:
		ShaderProgram();
		~ShaderProgram();

		GLuint get_ID() const { return ID; }

		void attach_shader(const Shader &shader);
		void link();
		void use() { glUseProgram(ID); }

		void load_from_file(const std::string &vertex_file_path,
							const std::string &fragment_file_path,
							const std::string &geometry_file_path = "");

		void set_uniform(const std::string &name, bool value) const;
		void set_uniform(const std::string &name, int value) const;
		void set_uniform(const std::string &name, float value) const;
		void set_uniform(const std::string &name, const glm::vec3 &value) const;
		void set_uniform(const std::string &name, const glm::vec4 &value) const;
		void set_uniform(const std::string &name, const glm::mat3 &value) const;
		void set_uniform(const std::string &name, const glm::mat4 &value) const;

	private:
		GLuint ID;

		GLint ShaderProgram::get_uniform_location(const std::string &name) const;
	};
}