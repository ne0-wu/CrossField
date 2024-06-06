#include "CrossField.h"

CrossField::CrossField(Mesh &input_mesh)
	: mesh(input_mesh),
	  local_frame(mesh), e_f_conj_pow4(mesh), x_f0(mesh)
{
}

void CrossField::set_constraints(const std::vector<Mesh::FaceHandle> &faces,
								 const std::vector<Eigen::Vector2d> &directions)
{
	if (faces.size() != directions.size())
		throw std::invalid_argument("The number of faces and directions must be the same");

	constraints_faces = faces;

	constraints_directions.resize(directions.size());
	for (size_t i = 0; i < directions.size(); ++i)
	{
		auto direc = directions[i].normalized();
		constraints_directions[i] = complexd(direc.x(), direc.y());
	}
}

void CrossField::solve()
{
	compute_local_frame();
	compute_LCconnection();
	solve_vector_field();
}

std::vector<Eigen::Vector3d[4]> CrossField::extract_cross_field()
{
	std::vector<Eigen::Vector3d[4]> cross_field(mesh.n_faces());

	for (const auto &f : mesh.faces())
	{
		auto u = local_frame[f].u;
		auto v = local_frame[f].v;

		double arg = std::arg(x_f0[f]) / 4;

		for (int k = 0; k < 4; ++k)
		{
			double angle = arg + k * M_PI / 2;
			cross_field[f.idx()][k] = cos(angle) * u + sin(angle) * v;
		}
	}

	return cross_field;
}

void CrossField::compute_local_frame()
{
	for (const auto &f : mesh.faces())
	{
		auto n = mesh.calc_face_normal(f);
		auto u = (mesh.point(f.halfedge().to()) - mesh.point(f.halfedge().from())).normalized();
		auto v = n.cross(u);
		local_frame[f] = {n, u, v};
	}
}

void CrossField::compute_LCconnection()
{
	for (const auto &e : mesh.edges())
	{
		if (e.is_boundary())
			continue;

		auto he = e.halfedge(0);
		auto f = he.face(), g = he.opp().face();

		auto he_direc = (mesh.point(he.to()) - mesh.point(he.to())).normalized();

		auto e_f = complexd(he_direc.dot(local_frame[f].u), he_direc.dot(local_frame[f].v));
		auto e_g = complexd(he_direc.dot(local_frame[g].u), he_direc.dot(local_frame[g].v));

		auto e_f_conj_pow4_val = std::pow(std::conj(e_f), 4);
		auto e_g_conj_pow4_val = std::pow(std::conj(e_g), 4);

		e_f_conj_pow4[he] = e_f_conj_pow4_val;
		e_f_conj_pow4[he.opp()] = e_f_conj_pow4_val;
	}
}

void CrossField::solve_vector_field()
{
	// Construct the matrix
	OpenMesh::FProp<bool> is_constraint_face(false, mesh);
	for (const auto &f : constraints_faces)
		is_constraint_face[f] = true;

	std::vector<Eigen::Triplet<complexd>> triplet_list;
	triplet_list.reserve(mesh.n_halfedges() * 4);

	for (const auto &f : mesh.faces())
	{
		if (is_constraint_face[f])
			// x_f0 == constraint
			triplet_list.push_back({f.idx(), f.idx(), 1.0});
		else
			// x_f0 * e_f_conj_pow4 - x_g0 * e_g_conj_pow4 == 0
			for (const auto &he : mesh.fh_range(f))
			{
				if (he.opp().is_boundary())
					continue;

				auto e = he.edge();
				auto f = he.face(), g = he.opp().face();

				auto e_f_conj_pow4_val = e_f_conj_pow4[he];
				auto e_g_conj_pow4_val = e_f_conj_pow4[he.opp()];

				triplet_list.push_back({f.idx(), f.idx(), e_f_conj_pow4_val});
				triplet_list.push_back({f.idx(), g.idx(), -e_g_conj_pow4_val});
			}
	}

	Eigen::SparseMatrix<complexd> A(mesh.n_faces(), mesh.n_faces());
	A.setFromTriplets(triplet_list.begin(), triplet_list.end());

	// Construct the right-hand side
	Eigen::VectorXcd b(mesh.n_faces());
	b.setZero();
	for (int i = 0; i < constraints_faces.size(); ++i)
		b[constraints_faces[i].idx()] = constraints_directions[i];

	// Solve the linear system
	Eigen::ConjugateGradient<Eigen::SparseMatrix<complexd>, Eigen::Lower | Eigen::Upper> solver;

	solver.compute(A);
	if (solver.info() != Eigen::Success)
		throw std::runtime_error("Failed to decompose the matrix");

	Eigen::VectorXcd x_f0_val = solver.solve(b);

	// Store the solution
	for (const auto &f : mesh.faces())
		x_f0[f] = x_f0_val[f.idx()];
}