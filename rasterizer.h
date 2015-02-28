#ifndef _RASTERIZER_H
#define _RASTERIZER_H

#include "fbo.h"
#include "base_math.h"
#include "geometry.h"

#define CLIPPING_DIST_NEAR 1
#define OCCLUSION_SKIP_RATE 4
#define FAST_OCCLUSION_TEST

class _rasterizer
{
public:
	static _rasterizer& singleton();
	void init(int a_srcw, int a_srch);
	void set_target(FBO &a_fbo);
	void set_mtlId( int a_id ){m_mtl_id = a_id;}
	void set_shader( void (*a_shader_func)(_fragment&, int&) )
		{ m_shader_func = a_shader_func; }
	void set_depthtest( bool a_val ){ m_do_depth_test =a_val;}
	void clear_scanline(const int y1, const int y2);
	inline int	get_screen_width() const{return m_screen_width;}
	inline int	get_screen_height() const{return m_screen_height;}
	int draw_triangle();
	void draw_polygon_edge( _vertex& a_v1, _vertex& a_v2 );
	int fill_polygon();
	int add_vertex_to_draw(const vec3f& v, const vec3f& nv, const vec2f& uv);
	vec3f transform_to_perspective( vec3f a_v );
	void clip_polygon_lessthan( float a_min, int a_clip_dim, _polygon& a_poly_src, _polygon& a_poly_dst );
	void clear_polygon_draw_counter();
	unsigned int get_polygon_drawed();
	void clear_scanline_fast( const int y1, const int y2 );
	void clip_polygon_lessthan_fast( float a_min, int a_clip_dim, _polygon_fast& a_poly_src, _polygon_fast& a_poly_dst );
	int draw_triangle_occlusion_test( _polygon_fast& a_src);
	void draw_polygon_edge_fast( vec3f a_v1, vec3f a_v2 );
	int	fill_polygon_fast();

private:
	static _rasterizer* m_singleton;
	int m_screen_width;
	int m_screen_height;

	bool m_do_depth_test;
	void (*m_shader_func)(_fragment&, int&);
	int	m_mtl_id;


	unsigned int s_polygon_drawed;

	FBO* m_render_target;
};

#endif