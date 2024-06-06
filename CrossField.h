#pragma once

#include <vector>
#include <complex>

#include <Eigen/Sparse>
#include <OpenMesh/Core/Utils/PropertyManager.hh>

#include "Mesh.h"

class CrossField
{
public:
	CrossField(Mesh &input_mesh);

	void set_constraints(const std::vector<Mesh::FaceHandle> &faces,
						 const std::vector<Eigen::Vector2d> &directions);

	void solve();

	std::vector<Eigen::Vector3d[4]> extract_cross_field();

private:
	using complexd = std::complex<double>;

	Mesh &mesh;

	std::vector<Mesh::FaceHandle> constraints_faces;
	std::vector<complexd> constraints_directions;

	// local frame on each face
	struct LocalFrame
	{
		Eigen::Vector3d n; // normal
		Eigen::Vector3d u;
		Eigen::Vector3d v;
	};

	OpenMesh::FProp<LocalFrame> local_frame;

	OpenMesh::HProp<complexd> e_f_conj_pow4; // LC connection

	OpenMesh::FProp<complexd> x_f0;

	void compute_local_frame();
	void compute_LCconnection();
	void solve_vector_field();
};