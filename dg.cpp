#include "dg.h"
#include "rasterizer.h"
#include "render.h"
_dgraph *_dgraph::m_singleton = 0;

_dgraph::_dgraph():m_root_node(0)
{
	m_root_node = new _dg_node;
}

_dgraph::~_dgraph()
{
	delete m_root_node;
}

_dgraph& _dgraph::singleton()
{
	if ( !m_singleton )
		m_singleton = new _dgraph;
	return *m_singleton;

}

void _dgraph::flush()
{
	if (m_root_node)
	{
		delete m_root_node;
	}
	m_root_node = new _dg_node;
	m_camera_orient_matrix.identity();
}

void _dgraph::render()
{
	mat4x4f model_mat;
	model_mat.identity();
	_rasterizer::singleton().set_depthtest(true);
	render_node(m_root_node, model_mat);

}

void _dgraph::render_node( _dg_node* a_node, mat4x4f a_parent_mat )
{
	mat4x4f model_mat = a_parent_mat * a_node->get_matrix();

	for ( int i=0; i<a_node->num_of_mesh(); ++i )
	{
		_render::singleton().render( a_node->get_mesh(i), model_mat, m_camera_orient_matrix );
	}

	for ( int i=0;i<a_node->num_of_child();++i )
	{
		render_node(&(a_node->get_child(i)), model_mat);
	}
}