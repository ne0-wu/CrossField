#include <OpenMesh/Core/IO/MeshIO.hh>

#include "Mesh.h"
#include "CrossField.h"

#include "MyGL/Window.h"
#include "MyGL/Mesh.h"
#include "MyGL/LineSegment.h"

#include <iostream>

std::tuple<double, Eigen::Vector3d>
incircle(const Eigen::Vector3d &A,
		 const Eigen::Vector3d &B,
		 const Eigen::Vector3d &C)
{
	// Compute the lengths of the sides of the triangle
	double a = (B - C).norm();
	double b = (A - C).norm();
	double c = (A - B).norm();

	// Compute the semi-perimeter of the triangle
	double s = (a + b + c) / 2.0;

	// Compute the area of the triangle using Heron's formula
	double area = std::sqrt(s * (s - a) * (s - b) * (s - c));

	// Compute the radius of the incircle
	double radius = area / s;

	// Compute the incenter coordinates
	Eigen::Vector3d incenter = (a * A + b * B + c * C) / (a + b + c);

	return {radius, incenter};
}

int main()
{
	// Cross field computation
	// =======================

	// Load mesh from file
	Mesh mesh;
	try
	{
		OpenMesh::IO::read_mesh(mesh, "data/models/camelhead.obj");
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	// Set constraints
	std::vector<Mesh::FaceHandle> constraints_faces;
	std::vector<Eigen::Vector2d> constraints_directions;
	constraints_faces.push_back(mesh.face_handle(0));
	constraints_directions.push_back(Eigen::Vector2d(1, 0));

	// Compute cross field
	CrossField cross_field(mesh);
	cross_field.set_constraints(constraints_faces, constraints_directions);
	cross_field.solve();
	auto cross_field_vectors = cross_field.extract_cross_field();

	// Create line segments for visualization
	OpenMesh::FProp<Eigen::Vector3d> incircle_center(mesh);
	double mean_radius = 0;
	for (const auto &f : mesh.faces())
	{
		auto [radius, center] = incircle(mesh.point(f.halfedge().from()),
										 mesh.point(f.halfedge().to()),
										 mesh.point(f.halfedge().next().to()));
		mean_radius += radius;
		incircle_center[f] = center;
	}
	mean_radius /= mesh.n_faces();

	std::vector<glm::vec3> ls_vertices;
	std::vector<GLuint> ls_indices;
	ls_vertices.resize(mesh.n_faces() * 5);
	ls_indices.resize(mesh.n_faces() * 10);
	for (const auto &f : mesh.faces())
	{
		ls_vertices[5 * f.idx() + 0] = {incircle_center[f].x(),
										incircle_center[f].y(),
										incircle_center[f].z()};
		for (int i = 0; i < 4; ++i)
		{
			auto to_vertex = incircle_center[f] + 0.5f * mean_radius * cross_field_vectors[f.idx()][i];
			ls_vertices[5 * f.idx() + i + 1] = {to_vertex.x(),
												to_vertex.y(),
												to_vertex.z()};
			ls_indices[10 * f.idx() + 2 * i] = 5 * f.idx();
			ls_indices[10 * f.idx() + 2 * i + 1] = 5 * f.idx() + i + 1;
		}
	}

	// Visualize
	// =========

	// Initialize window (and OpenGL context)
	MyGL::Window window(1500, 1500, "Cross Field Visualization");
	float last_frame_time = 0.0f;
	float delta_time = 0.0f;

	// Convert mesh to MyGL::Mesh
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;

	glm::vec3 center = {0.0f, 0.0f, 0.0f};

	vertices.reserve(mesh.n_vertices());
	for (const auto &vertex : mesh.vertices())
	{
		vertices.push_back({mesh.point(vertex).x(),
							mesh.point(vertex).y(),
							mesh.point(vertex).z()});
		center += vertices.back();
	}
	center /= static_cast<float>(mesh.n_vertices());

	indices.reserve(mesh.n_faces() * 3);
	for (const auto &face : mesh.faces())
		for (const auto &vertex : mesh.fv_range(face))
			indices.push_back(vertex.idx());

	MyGL::Mesh gl_mesh(vertices, indices);

	// Construct line segments
	MyGL::LineSegment ls(ls_vertices, ls_indices);

	// Load shader from file
	MyGL::ShaderProgram basic;
	try
	{
		basic.load_from_file("data/shaders/basic.vert", "data/shaders/basic.frag");
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	// Set up camera
	MyGL::OrbitCamera camera(center);
	camera.set_position({0.0f, 0.0f, 1.0f});

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Main loop
	while (!window.should_close())
	{
		// per-frame time logic
		float current_frame_time = static_cast<float>(glfwGetTime());
		delta_time = current_frame_time - last_frame_time;
		last_frame_time = current_frame_time;

		// input
		window.process_input();
		window.process_input_camera(camera, delta_time);

		camera.look_at(center);

		// update mvp matrices
		basic.use();

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.get_view_matrix() * camera.get_model_matrix();
		auto [width, height] = window.get_framebuffer_size();
		glm::mat4 projection = camera.get_projection_matrix(static_cast<float>(width) / height);
		basic.set_uniform("model", model);
		basic.set_uniform("view", view);
		basic.set_uniform("projection", projection);

		// render
		// ======
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// mesh
		basic.set_uniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		gl_mesh.draw_wireframe();
		basic.set_uniform("color", glm::vec4(1.0f, 0.5f, 0.2f, 1.0f));
		gl_mesh.draw();

		// vector field
		basic.set_uniform("color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		ls.draw();

		// swap buffers and poll events
		window.swap_buffers();
		window.poll_events();
	}

	return 0;
}