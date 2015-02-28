#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include <smmintrin.h>
#include "str.h"
#include "base_math.h"

union _vertex
{
	struct {
		float x,y,z;
		float nx,ny,nz;
		float u,v;
	}; 
	float cell[8];
	__m128 sse_cell[ 2 ];
};

struct _polygon_fast
{
	vec3f v[6];
	int vc;
};

struct _polygon
{

	_polygon& operator=(_polygon& a_p)
	{
		memcpy((void*)v, (void*)a_p.v, sizeof(_vertex)*6);
		vc = a_p.vc;
		return *this;
	}
	_vertex v[6];
	int	vc;
};

struct _fragment
{
	int mtl_id;
	void (*shader)( _fragment&, int& );
	_vertex cell;
};

struct _texture
{
	_texture():height(0),width(0),pixels(0),data(0){}
	int height, width, pixels;
	int *data;
	str name;
};

struct _face
{
	int v[3];
	int nv[3];
	int uv[3];
};

struct _segment
{
	_segment():face_start(0),face_end(0),mtlId(0),
		max_dist_to_draw(99999){}
	int face_start;
	int face_end;
	int mtlId;
	
	float max_dist_to_draw;

	str mtl_name;

	vec3f box_local[ 8 ];
	vec3f box_local_center;
	vec3f box_model[ 8 ];
	vec3f box_model_center;
	vec3f box_proj[ 8 ];
	vec3f box_proj_center;

	float diag;
};

struct _mesh
{
	_mesh() :
		seg(0), sc(0),
		face(0), fc(0),
		vertex(0), normal(0), uv(0),
		vc(0), nc(0), uvc(0){}

	_segment **seg; //for sort
	int sc;

	_face *face;
	int fc;

	vec3f *vertex;
	int vc;

	vec3f *normal;
	int nc;

	vec2f *uv;
	int uvc;



};


#endif