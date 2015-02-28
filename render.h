#ifndef _RENDER_H
#define _RENDER_H
#include "geometry.h"
#include "base_math.h"

class _render
{
public:
	static _render& singleton();
	void render( _mesh& a_mesh, mat4x4f a_model_matrix, mat4x4f a_camera_matrix );
	void vertices_transform( _mesh& a_mesh, mat4x4f a_model_matrix, mat4x4f a_camera_matrix );
	void render_segment( _mesh& a_mesh, int a_seg_id );
	void recompute_mesh_box(_mesh& a_mesh, mat4x4f a_model_matrix,mat4x4f  a_camera_matrix);
	void sort_segment( _segment** a_segs, int a_segc );
	int	 occlusion_test( vec3f *a_vv );

private:
	_render();
	~_render();
	static _render* m_singleton;

};

#endif