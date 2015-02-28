#ifndef _DG_NODE_H
#define _DG_NODE_H

#include <vector>
#include "base_math.h"
#include "geometry.h"

using namespace std;

class _dg_node
{
public:
	_mesh& get_mesh(int i);
	void add_mesh( _mesh* a_mesh);
	_dg_node& get_child(int i);
	int num_of_mesh();
	int num_of_child();
	mat4x4f& get_matrix();
private:
	vector<_dg_node*> m_child_stack;
	mat4x4f m_transform_matrix;
	vector<_mesh*>	m_mesh_stack;

};

#endif