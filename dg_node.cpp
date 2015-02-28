#include "dg_node.h"

_mesh& _dg_node::get_mesh(int i)
{
	return *m_mesh_stack[i];
}

_dg_node& _dg_node::get_child(int i)
{
	return *m_child_stack[i];
}

void _dg_node::add_mesh(_mesh* a_mesh)
{
	m_mesh_stack.push_back(a_mesh);
}

int _dg_node::num_of_mesh()
{
	return m_mesh_stack.size();
}

int _dg_node::num_of_child()
{
	return m_child_stack.size();
}

mat4x4f& _dg_node::get_matrix()
{
	return m_transform_matrix;
}

