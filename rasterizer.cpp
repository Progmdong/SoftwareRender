#include <assert.h>
#include "rasterizer.h"

//Assume no resolution higher than 1024
static _vertex s_scanline_head[1024];
static _vertex s_scanline_tail[1024];
static vec3f s_scanline_head_fast[1024];
static vec3f s_scanline_tail_fast[1024];

static int s_scan_start;
static int s_scan_end;


static _polygon s_polygon;
static _polygon s_polygon_clippedz;
static _polygon s_polygon_clipped;

static _polygon_fast s_polygon_fast;
static _polygon_fast s_polygon_fast_clipped;

static float s_perspective_constant;

_rasterizer* _rasterizer::m_singleton = 0;
_rasterizer& _rasterizer::singleton()
{
	if (!m_singleton)
	{
		m_singleton = new _rasterizer;
	}
	return *m_singleton;
}

void _rasterizer::clear_polygon_draw_counter()
{
	s_polygon_drawed = 0;
}

unsigned int _rasterizer::get_polygon_drawed()
{
	return s_polygon_drawed;
}

void _rasterizer::init(int a_srcw, int a_srch)
{
	m_screen_width = a_srcw;
	m_screen_height = a_srch;
}

void _rasterizer::set_target(FBO &a_fbo)
{
	m_render_target = &a_fbo;
	float fov = 1.f;
	s_perspective_constant = 1. / m_screen_width * fov;
}

void _rasterizer::clear_scanline(const int y1, const int y2)
{

	if ( m_render_target==nullptr )
	{
		return;
	}

	for ( int y=y1; y<y2; ++y )
	{
		s_scanline_head[y].x = (float)m_render_target->texture.width;
		s_scanline_tail[y].x = 0;
	}

}

void _rasterizer::clear_scanline_fast( const int y1, const int y2 )
{
	if ( m_render_target==nullptr )
	{
		return;
	}

	for ( int y=y1; y<y2; ++y )
	{
		s_scanline_head_fast[y].x = (float)m_render_target->texture.width;
		s_scanline_tail_fast[y].x = 0;
	}
}

vec3f _rasterizer::transform_to_perspective( vec3f a_v )
{
	//used for clipping test
	a_v.z += (a_v.z==0);
	float sign = a_v.z>0?1.f:-1.f;
	float factor = 1.f / fabsf( a_v.z*s_perspective_constant );
	float x = a_v.x*factor + get_screen_width()/2;
	float y = a_v.y*factor + get_screen_height()/2;
	float z = sign * factor;
	return vec3f(x,y,z);
}

int _rasterizer::add_vertex_to_draw(const vec3f& v, const vec3f& nv, const vec2f& uv)
{
	int& vc = s_polygon.vc;
	s_polygon.v[vc].x = v.x;
	s_polygon.v[vc].y = v.y;
	s_polygon.v[vc].z = v.z;
	s_polygon.v[vc].nx = nv.x;
	s_polygon.v[vc].ny = nv.y;
	s_polygon.v[vc].nz = nv.z;
	s_polygon.v[vc].u = uv.u;
	s_polygon.v[vc].v = uv.v;

	++vc;
	if ( vc<3 )
	{
		return 0;
	}

	int pixel_filled = draw_triangle();
	s_polygon.vc = 0;
	return pixel_filled;

}

int _rasterizer::draw_triangle()
{
	assert( m_render_target );
	int pixel_filled = 0;
	
	clip_polygon_lessthan(CLIPPING_DIST_NEAR, 2, s_polygon, s_polygon_clippedz);
	if(s_polygon_clippedz.vc==0)
		return pixel_filled;

	//to perspective
	for ( int i=0; i<s_polygon_clippedz.vc;i++ )
	{
		s_polygon_clippedz.v[i].z = 1.f / ( s_polygon_clippedz.v[i].z * s_perspective_constant );
		s_polygon_clippedz.v[i].x = s_polygon_clippedz.v[i].x * s_polygon_clippedz.v[i].z + get_screen_width()/2;
		s_polygon_clippedz.v[i].y = s_polygon_clippedz.v[i].y * s_polygon_clippedz.v[i].z + get_screen_height()/2;
		s_polygon_clippedz.v[i].u = s_polygon_clippedz.v[i].u * s_polygon_clippedz.v[i].z;
		s_polygon_clippedz.v[i].v = s_polygon_clippedz.v[i].v * s_polygon_clippedz.v[i].z;

	}

	//Cull away back face
	const float n = (s_polygon_clippedz.v[0].x - s_polygon_clippedz.v[1].x)*(s_polygon_clippedz.v[2].y - s_polygon_clippedz.v[1].y)
		- (s_polygon_clippedz.v[0].y - s_polygon_clippedz.v[1].y)*(s_polygon_clippedz.v[2].x - s_polygon_clippedz.v[1].x);
	if ( n<0. )
	{
		//back face
		return pixel_filled;
	}

	float f_xmax = -99999;
	float f_xmin = 99999;
	float f_ymax = -99999;
	float f_ymin = 99999;
	for ( int v=0; v<s_polygon_clippedz.vc;v++ )
	{
		f_xmax = MAX( f_xmax, s_polygon_clippedz.v[v].x );
		f_xmin = MIN( f_xmin, s_polygon_clippedz.v[v].x );
		f_ymax = MAX( f_ymax, s_polygon_clippedz.v[v].y );
		f_ymin = MIN( f_ymin, s_polygon_clippedz.v[v].y );
	}

	int ymax = (int)f_ymax;
	int ymin = (int)f_ymin;

	if ( ymax - ymin==0 )
	{
		return pixel_filled;
	}

	int xmax = (int)f_xmax;
	int xmin = (int)f_xmin;

	if ( xmax<=0 || xmin >= get_screen_width() || ymax<=0 || ymin >= get_screen_height() )
	{
		// Polygon outside
		return pixel_filled;
	}

	//Just Cull left and top side
	int cull_x_code = (	(xmax>=0) & (xmin<0) );
	int cull_y_code = ( (ymax>=0) & (ymin<0) );
	int cull_code = (cull_y_code<<1) + cull_x_code;


	switch (cull_code)
	{
	case 1:
		//Cull x
		clip_polygon_lessthan( 0, 0, s_polygon_clippedz, s_polygon_clipped );
		break;
	case 2:
		//Cull y
		clip_polygon_lessthan( 0,1,s_polygon_clippedz, s_polygon_clipped );
		break;
	case 3:
		_polygon tmp_polygon;
		clip_polygon_lessthan( 0,0, s_polygon_clippedz, tmp_polygon );
		clip_polygon_lessthan(0,1, tmp_polygon, s_polygon_clipped);
		break;
	default:
		for(int i=0; i<s_polygon_clippedz.vc; i++)
		{
			s_polygon_clipped.v[i].sse_cell[0] = s_polygon_clippedz.v[i].sse_cell[0];
			s_polygon_clipped.v[i].sse_cell[1] = s_polygon_clippedz.v[i].sse_cell[1];	
		}
		s_polygon_clipped.vc = s_polygon_clippedz.vc;
		break;
	}

	s_scan_start = MAX( ymin, 0 );
	s_scan_end = MIN(ymax, get_screen_height());
	clear_scanline( s_scan_start, s_scan_end );

	for ( int i=0; i<s_polygon_clipped.vc; i++ )
	{
		draw_polygon_edge( s_polygon_clipped.v[i], s_polygon_clipped.v[ (i+1)%s_polygon_clipped.vc ] );
	}

	pixel_filled = fill_polygon();
	s_polygon_drawed++;
	return pixel_filled;

}

void _rasterizer::draw_polygon_edge( _vertex& a_v1, _vertex& a_v2 )
{
	_vertex v1 = a_v1;
	_vertex v2 = a_v2;
	if ( v1.y > v2.y )
	{
		std::swap( v1.sse_cell[0], v2.sse_cell[0] );
		std::swap( v1.sse_cell[1], v2.sse_cell[1] );
	}

	const float delta = 1.f / ( v2.y - v1.y );
	_vertex vd;
	vd.sse_cell[0] = _mm_mul_ps( _mm_sub_ps(v2.sse_cell[0], v1.sse_cell[0]), _mm_set_ps1(delta) );
	vd.sse_cell[1] = _mm_mul_ps( _mm_sub_ps(v2.sse_cell[1], v1.sse_cell[1]), _mm_set_ps1(delta) );

	const float corr = ceilf( v1.y ) - v1.y;
	_vertex v0;
	v0.sse_cell[0] = _mm_add_ps( v1.sse_cell[0], _mm_mul_ps( vd.sse_cell[0], _mm_set_ps1(corr) ) );
	v0.sse_cell[1] = _mm_add_ps( v1.sse_cell[1], _mm_mul_ps( vd.sse_cell[1], _mm_set_ps1(corr) ) );

	int y1 = (int)v1.y;
	int y2 = (int)v2.y;		
	y2 = MIN( y2, get_screen_height() );

	for ( int y=y1; y<y2; ++y )
	{
		_vertex& head = s_scanline_head[y];
		_vertex& tail = s_scanline_tail[y];

		if ( v0.x < head.x )
		{
			head.sse_cell[0] = v0.sse_cell[0];
			head.sse_cell[1] = v0.sse_cell[1];
		}
		if ( v0.x > tail.x )
		{
			tail.sse_cell[0] = v0.sse_cell[0];
			tail.sse_cell[1] = v0.sse_cell[1];
		}
		//next line
		v0.sse_cell[0] = _mm_add_ps( v0.sse_cell[0], vd.sse_cell[0] );
		v0.sse_cell[1] = _mm_add_ps(v0.sse_cell[1], vd.sse_cell[1]);
	}

}

int _rasterizer::fill_polygon()
{
	int pixel_filled = 0;
	for ( int y= s_scan_start; y<s_scan_end; y++ )
	{
		int xmin = int(s_scanline_head[y].x);
		int xmax = int(s_scanline_tail[y].x);

		_vertex v1 = s_scanline_head[y];
		_vertex v2 = s_scanline_tail[y];
		_vertex vd;
		const float delta = 1.f / (v2.x - v1.x);
		vd.sse_cell[0] = _mm_mul_ps( _mm_sub_ps(v2.sse_cell[0] , v1.sse_cell[0]), _mm_set_ps1(delta) );
		vd.sse_cell[1] = _mm_mul_ps( _mm_sub_ps(v2.sse_cell[1], v1.sse_cell[1]), _mm_set_ps1(delta) );
		const float corr = ceilf(v1.x) - v1.x;
		v1.sse_cell[0] = _mm_add_ps( v1.sse_cell[0], _mm_mul_ps(vd.sse_cell[0], _mm_set_ps1(corr)) ); 
		v1.sse_cell[1] = _mm_add_ps(v1.sse_cell[1], _mm_mul_ps( vd.sse_cell[1], _mm_set_ps1(corr) ));
		_fragment *frag = &( m_render_target->dual_buffer[0][y*get_screen_width()] );

		xmax = MIN(xmax, get_screen_width());

		if ( m_do_depth_test )
		{
			for ( int x=xmin; x<xmax; x++ )
			{
				if (  frag[x].cell.z <= v1.z )
				{
					frag[x].shader = m_shader_func;
					frag[x].mtl_id = m_mtl_id;
					frag[x].cell.sse_cell[0] = v1.sse_cell[0];
					frag[x].cell.sse_cell[1] = v1.sse_cell[1];
					pixel_filled++;
				}

				v1.sse_cell[0] = _mm_add_ps( v1.sse_cell[0], vd.sse_cell[0]);
				v1.sse_cell[1] = _mm_add_ps( v1.sse_cell[1], vd.sse_cell[1]);
			}
		}
		else
		{
			for ( int x=xmin; x<xmax; x++ )
			{
				frag[x].shader = m_shader_func;
				frag[x].mtl_id = m_mtl_id;
				frag[x].cell.sse_cell[0] = v1.sse_cell[0];
				frag[x].cell.sse_cell[1] = v1.sse_cell[1];
				v1.sse_cell[0] = _mm_add_ps( v1.sse_cell[0], vd.sse_cell[0]);
				v1.sse_cell[1] = _mm_add_ps( v1.sse_cell[1], vd.sse_cell[1]);
				pixel_filled++;
			}
		}

	}
	return pixel_filled;
}

void _rasterizer::clip_polygon_lessthan( float a_min, int a_clip_dim, _polygon& a_poly_src, _polygon& a_poly_dst )
{
	a_poly_dst.vc = 0;
	for ( int v=0; v<a_poly_src.vc; ++v )
	{
		_vertex &v1 = a_poly_src.v[v];
		_vertex &v2 = a_poly_src.v[(v+1)%a_poly_src.vc];
		if ( v1.cell[a_clip_dim]<a_min && v2.cell[a_clip_dim]<a_min )
		{
			continue;
		}
		if ( v2.cell[a_clip_dim]<a_min )
		{
			_vertex vd;

			vd.sse_cell[0] = _mm_sub_ps( v1.sse_cell[0], v2.sse_cell[0] );
			vd.sse_cell[1] = _mm_sub_ps( v1.sse_cell[1], v2.sse_cell[1] );
			const float d = vd.cell[a_clip_dim];
			vd.sse_cell[0] = _mm_div_ps(vd.sse_cell[0],_mm_set_ps1(d));
			vd.sse_cell[1] = _mm_div_ps(vd.sse_cell[1],_mm_set_ps1(d));
			const float dclip = (a_min - v2.cell[a_clip_dim]);
			a_poly_dst.v[ a_poly_dst.vc++ ] = v1;
			a_poly_dst.v[a_poly_dst.vc].sse_cell[0] = _mm_add_ps( _mm_mul_ps(vd.sse_cell[0], _mm_set_ps1(dclip)),v2.sse_cell[0] );
			a_poly_dst.v[a_poly_dst.vc].sse_cell[1] = _mm_add_ps( _mm_mul_ps(vd.sse_cell[1], _mm_set_ps1(dclip)),v2.sse_cell[1] );
			a_poly_dst.vc++;
			continue;
		}
		if ( v1.cell[a_clip_dim]<a_min )
		{
			_vertex vd;

			vd.sse_cell[0] = _mm_sub_ps( v2.sse_cell[0], v1.sse_cell[0] );
			vd.sse_cell[1] = _mm_sub_ps( v2.sse_cell[1], v1.sse_cell[1] );

			const float d = vd.cell[a_clip_dim];
			vd.sse_cell[0] = _mm_div_ps(vd.sse_cell[0],_mm_set_ps1(d));
			vd.sse_cell[1] = _mm_div_ps(vd.sse_cell[1],_mm_set_ps1(d));
			const float dclip = (a_min - v1.cell[a_clip_dim]);
			a_poly_dst.v[a_poly_dst.vc].sse_cell[0] = _mm_add_ps( _mm_mul_ps(vd.sse_cell[0], _mm_set_ps1(dclip)),v1.sse_cell[0] );
			a_poly_dst.v[a_poly_dst.vc].sse_cell[1] = _mm_add_ps( _mm_mul_ps(vd.sse_cell[1], _mm_set_ps1(dclip)),v1.sse_cell[1] );
			a_poly_dst.vc++;
			continue;
		}
		a_poly_dst.v[ a_poly_dst.vc ].sse_cell[0] = v1.sse_cell[0];
		a_poly_dst.v[ a_poly_dst.vc ].sse_cell[1] = v1.sse_cell[1];
		a_poly_dst.vc++;
	}
}

void _rasterizer::clip_polygon_lessthan_fast( float a_min, int a_clip_dim, _polygon_fast& a_poly_src, _polygon_fast& a_poly_dst )
{
	a_poly_dst.vc= 0;

	for ( int v=0; v<a_poly_src.vc; v++ )
	{

		vec3f &v1 = a_poly_src.v[v];
		vec3f &v2 = a_poly_src.v[ (v+1)%a_poly_src.vc ];

		if ( v1.cell[a_clip_dim]<a_min && v2.cell[a_clip_dim]<a_min )
		{
			continue;
		}
		if ( v2.cell[a_clip_dim]<a_min )
		{
			vec3f vd = v1 - v2;
			const float d = (a_min - v2.cell[a_clip_dim])/(vd.cell[a_clip_dim]);
			a_poly_dst.v[ a_poly_dst.vc++ ] = v1;
			a_poly_dst.v[ a_poly_dst.vc++ ] = v2 + vd*d;
			continue;
		}
		if ( v1.cell[a_clip_dim]<a_min )
		{
			vec3f vd = v2 - v1;
			const float d = (a_min - v1.cell[a_clip_dim])/(vd.cell[a_clip_dim]);
			a_poly_dst.v[ a_poly_dst.vc++ ] = v1 + vd*d;
			continue;
		}

		a_poly_dst.v[a_poly_dst.vc++] = v1;

	}
}

int _rasterizer::draw_triangle_occlusion_test( _polygon_fast& a_src )
{
	int pixel_filled = 0;
	
	clip_polygon_lessthan_fast( CLIPPING_DIST_NEAR, 2, a_src, s_polygon_fast);

	if(s_polygon_fast.vc==0)
		return pixel_filled;

	//To perspective
	for ( int v=0; v<s_polygon_fast.vc; ++v )
	{
		s_polygon_fast.v[v].z = 1.f / ( s_polygon_fast.v[v].z * s_perspective_constant );
		s_polygon_fast.v[v].x = s_polygon_fast.v[v].x * s_polygon_fast.v[v].z + get_screen_width()/2;
		s_polygon_fast.v[v].y = s_polygon_fast.v[v].y * s_polygon_fast.v[v].z + get_screen_height()/2;
	}

	float f_xmax = -99999;
	float f_xmin = 99999;
	float f_ymax = -99999;
	float f_ymin = 99999;
	for ( int v=0; v<s_polygon_fast.vc;v++ )
	{
		f_xmax = MAX( f_xmax, s_polygon_fast.v[v].x );
		f_xmin = MIN( f_xmin, s_polygon_fast.v[v].x );
		f_ymax = MAX( f_ymax, s_polygon_fast.v[v].y );
		f_ymin = MIN( f_ymin, s_polygon_fast.v[v].y );
	}

	int ymax = (int)f_ymax;
	int ymin = (int)f_ymin;

	if ( ymax - ymin==0 )
	{
		return pixel_filled;
	}

	int xmax = (int)f_xmax;
	int xmin = (int)f_xmin;

	if ( xmax<=0 || xmin >= get_screen_width() || ymax<=0 || ymin >= get_screen_height() )
	{
		// Polygon outside
		return pixel_filled;
	}

	//Just Cull left and top side
	int cull_x_code = ( (xmax>=0) & (xmin<0) );
	int cull_y_code = ( (ymax>=0) & (ymin<0) );
	int cull_code = (cull_y_code<<1) + cull_x_code;

	_polygon_fast tmp_polygon;
	switch (cull_code)
	{
	case 1:
		//Cull x
		clip_polygon_lessthan_fast( 0, 0, s_polygon_fast, s_polygon_fast_clipped );
		break;
	case 2:
		//Cull y
		clip_polygon_lessthan_fast( 0,1,s_polygon_fast, s_polygon_fast_clipped );
		break;
	case 3:
		clip_polygon_lessthan_fast( 0,0, s_polygon_fast, tmp_polygon );
		clip_polygon_lessthan_fast(0,1, tmp_polygon, s_polygon_fast_clipped);
		break;
	default:
		for (int i=0; i<s_polygon_fast.vc; i++)
		{
			s_polygon_fast_clipped.v[i] = s_polygon_fast.v[i];
		}
		s_polygon_fast_clipped.vc = s_polygon_fast.vc;
		break;
	}

	s_scan_start = MAX(ymin, 0);
	s_scan_end = MIN(ymax, get_screen_height());

	clear_scanline_fast( s_scan_start,s_scan_end );

	for ( int v=0; v<s_polygon_fast_clipped.vc;v++)
	{
		draw_polygon_edge_fast( s_polygon_fast_clipped.v[v], s_polygon_fast_clipped.v[ (v+1)%s_polygon_fast_clipped.vc ] );
	}

	pixel_filled = fill_polygon_fast();

	return pixel_filled;

}

void _rasterizer::draw_polygon_edge_fast( vec3f a_v1, vec3f a_v2 )
{
	if ( a_v2.y < a_v1.y )
	{
		vec3f t = a_v1;
		a_v1 = a_v2;
		a_v2 = t;
	}
	const float delta = 1.f / ( a_v2.y - a_v1.y );
	vec3f vd;
	vd = (a_v2-a_v1)*delta;

	const float corr = ceilf( a_v1.y ) - a_v1.y;
	vec3f v0;
	v0 = a_v1 + vd*corr;

	int y1 = (int)a_v1.y;
	int y2 = (int)a_v2.y;
	y2 = MIN( y2, get_screen_height() );

	for ( int y=y1; y<y2; ++y )
	{
		vec3f& head = s_scanline_head_fast[y];
		vec3f& tail = s_scanline_tail_fast[y];

		if ( v0.x < head.x )
		{
			head = v0;
		}
		if ( v0.x > tail.x )
		{
			tail = v0;
		}
		//next line
		v0 = v0 + vd;
	}

	
}

int _rasterizer::fill_polygon_fast()
{
	int pixel_filled = 0;
	for ( int y= s_scan_start; y<s_scan_end; y+=OCCLUSION_SKIP_RATE )
	{
		int xmin = int(s_scanline_head_fast[y].x);
		int xmax = int(s_scanline_tail_fast[y].x);
		if(xmin==xmax)continue;
		vec3f v1 = s_scanline_head_fast[y];
		vec3f v2 = s_scanline_tail_fast[y];
		vec3f vd;
		const float delta = 1.f / (v2.x - v1.x);
		vd = (v2 - v1)*delta;

		const float corr = ceilf(v1.x) - v1.x;
		v1 = v1 + vd*corr;
		_fragment *frag = &( m_render_target->dual_buffer[0][y*get_screen_width()] );

		xmax = MIN(xmax, get_screen_width());

		for ( int x=xmin; x<xmax; x+=OCCLUSION_SKIP_RATE )
		{
			if (  frag[x].cell.z <= v1.z )
			{
#ifdef FAST_OCCLUSION_TEST
				return 1;
#else
				pixel_filled++;
#endif
			}

			v1 = v1 + vd;
		}

	}
	return pixel_filled;
}