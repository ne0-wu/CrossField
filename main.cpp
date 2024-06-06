#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Core>
#include <OpenMesh/Core/IO/MeshIO.hh>

#include "Mesh.h"
#include "CrossField.h"

int main()
{
	// Load mesh from file
	Mesh input_mesh;
	if (!OpenMesh::IO::read_mesh(input_mesh, "data/models/cathead.obj"))
	{
		std::cerr << "Error loading mesh from file input.obj" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Set constraints
	std::vector<Mesh::FaceHandle> constraints_faces;
	std::vector<Eigen::Vector2d> constraints_directions;
	constraints_faces.push_back(input_mesh.face_handle(0));
	constraints_directions.push_back(Eigen::Vector2d(1, 0));

	// Compute cross field
	CrossField cross_field(input_mesh);
	cross_field.set_constraints(constraints_faces, constraints_directions);
	cross_field.solve();
	auto cross_field_vectors = cross_field.extract_cross_field();

	return 0;
}