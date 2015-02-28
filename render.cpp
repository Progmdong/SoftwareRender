#include "render.h"
#include "rasterizer.h"
#include "material_mgr.h"


#define  MAX_VERTICES_PER_MESH (1<<19)
static vec3f s_v[ MAX_VERTICES_PER_MESH ];
static vec3f s_nv[ MAX_VERTICES_PER_MESH ];

_render* _render::m_singleton = 0;
_render& _render::singleton()
{
	return *m_singleton;
}

void _render::sort_segment( _segment** a_segs, int a_segc )
{
	//sort depend on depth.
#define DEPTH_OF_SEG( s )  s->box_proj_center.z

	bool done = false;
	int tail = a_segc;
	while ( !done )
	{
		done = true;
		for ( int i=0; i<tail-1; ++i )
		{
			if ( DEPTH_OF_SEG(a_segs[i]) < DEPTH_OF_SEG(a_segs[i+1]) )
			{
				std::swap( a_segs[i], a_segs[i+1] );
				done = false;
			}
		}
		tail--;
	}

#undef DEPTH_OF_SEG
}

void _render::recompute_mesh_box(_mesh& a_mesh, mat4x4f a_model_matrix,mat4x4f a_camera_matrix)
{
	mat4x4f tfm_mat = a_camera_matrix * a_model_matrix;
	for ( int s=0; s<a_mesh.sc; ++s )
	{
		_segment& seg = *a_mesh.seg[s];

		seg.box_model_center = tfm_mat.transform( seg.box_local_center );
		seg.box_proj_center = _rasterizer::singleton().transform_to_perspective(seg.box_model_center);

		for (int i=0; i<8;i++)
		{
			seg.box_model[ i ] = tfm_mat.transform( seg.box_local[i] );
			seg.box_proj[ i ] = _rasterizer::singleton().transform_to_perspective(seg.box_model[i]);
		}
	}

	sort_segment( a_mesh.seg, a_mesh.sc );

}


void _render::render( _mesh& a_mesh, mat4x4f a_model_matrix, mat4x4f a_camera_matrix )
{
	recompute_mesh_box( a_mesh, a_model_matrix, a_camera_matrix );

	float scr_w = (float)_rasterizer::singleton().get_screen_width();
	float scr_h = (float)_rasterizer::singleton().get_screen_height();

	bool vertices_has_transformed = false;

	for ( int s=0; s<a_mesh.sc; ++s )
	{
		_segment *cur_seg = a_mesh.seg[s];

		// Check if the segment is in perspective
		float minx,miny,minz;
		float maxx,maxy,maxz;
		minx = miny = minz = 99999;
		maxx = maxy = maxz = -99999;
		for ( int c=0; c<8;c++ )
		{
			minx = MIN( minx, cur_seg->box_proj[c].x );
			miny = MIN( miny, cur_seg->box_proj[c].y );
			minz = MIN( minz, cur_seg->box_model[c].z );
			maxx = MAX( maxx, cur_seg->box_proj[c].x);
			maxy = MAX( maxy, cur_seg->box_proj[c].y);
			maxz = MAX( maxx, cur_seg->box_model[c].z);
		}
		if ( maxx<0 || maxy<0 || minx>scr_w || miny>scr_h || maxz<0 || minz > cur_seg->max_dist_to_draw )
			continue;

		if ( !vertices_has_transformed )
		{
			vertices_transform( a_mesh, a_model_matrix, a_camera_matrix );
			vertices_has_transformed = true;
		}

		//Check if segment is occluded only if containing more than 64 polygons
		if ( cur_seg->face_end - cur_seg->face_start > 64 )
		{
			const int pixels_filled = occlusion_test( cur_seg->box_model );
			if ( pixels_filled==0 )
			{
				continue;
			}
		}

		render_segment( a_mesh, s );

	}

}

int _render::occlusion_test( vec3f *a_vv )
{
	static const int s_cube_indeces[36] = {0,1,2,
											1,3,2,
											1,5,7,
											1,7,3,
											3,7,6,
											3,6,2,
											0,2,4,
											2,6,4,
											4,6,5,
											6,7,5,
											0,4,1,
											4,5,1};
	_polygon_fast poly;
	int pixel_filled = 0;
	for ( int i=0; i<36; i+=3 )
	{
		poly.vc = 3;
		poly.v[0] = a_vv[ s_cube_indeces[i] ];
		poly.v[1] = a_vv[ s_cube_indeces[i+1] ];
		poly.v[2] = a_vv[ s_cube_indeces[i+2] ];
		pixel_filled += _rasterizer::singleton().draw_triangle_occlusion_test(poly);
	}
	return pixel_filled;
}

void _render::render_segment( _mesh& a_mesh, int a_seg_id )
{
	_rasterizer::singleton().set_mtlId( a_mesh.seg[a_seg_id]->mtlId );
	_material *mtl = _material_mgr::singleton().get_material(a_mesh.seg[a_seg_id]->mtlId); 
	_rasterizer::singleton().set_shader( mtl->shader );
	_rasterizer::singleton().set_depthtest( true );

	for ( int f=a_mesh.seg[a_seg_id]->face_start; f<a_mesh.seg[a_seg_id]->face_end; ++f )
	{
		for ( int v=0; v<3; v++ )
		{
			_rasterizer::singleton().add_vertex_to_draw( s_v[a_mesh.face[f].v[v]], 
				s_nv[a_mesh.face[f].nv[v]], 
				a_mesh.uv[ a_mesh.face[f].uv[v] ] );
		}
	}
}

void _render::vertices_transform( _mesh& a_mesh, mat4x4f a_model_matrix, mat4x4f a_camera_matrix )
{

	mat4x4f view_model_mat = a_camera_matrix * a_model_matrix;

	for( int v=0; v<a_mesh.vc; ++v )
	{
		s_v[v] = view_model_mat.transform( a_mesh.vertex[v] );
	}

	for( int nv=0; nv<a_mesh.nc; ++nv )
	{

		s_nv[nv] = a_model_matrix.transform_rotation(a_mesh.normal[nv]);
	}
}