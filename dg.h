#ifndef _DG_H
#define _DG_H
#include "base_math.h"
#include "dg_node.h"

class _dgraph
{

public:
	static _dgraph& singleton();

	_dg_node* get_root_node(){return m_root_node;}
	void render();
	void flush();
	void render_node( _dg_node* a_node, mat4x4f a_parent_mat );
	void set_camera_matrix(mat4x4f a_camera_mat){m_camera_orient_matrix = a_camera_mat;}
private:
	_dgraph();
	~_dgraph();

	static _dgraph *m_singleton;

	_dg_node	*m_root_node;

	mat4x4f		m_camera_orient_matrix;

};

#endif