#include "soft_shader.h"
#include "material_mgr.h"
#include "tex_mgr.h"

static int GetBilinearColor(float u, float v,	const _texture &tex)
{
#define BILINEAR_SHIFT 15
#define BILINEAR_SHIFT_VALUE (1<<BILINEAR_SHIFT)
	//Put in texture range 0 - 1
	u = u + 512;
	v = v + 512;
	u = u - int(u);
	v = v - int(v);

	float txf = u * tex.width ;
	float tyf = v * tex.height ;


	const int tx = CLAMP(int(txf), 0, tex.width - 1);
	const int ty = CLAMP(int(tyf), 0, tex.height - 1);

	const int fu	= int( (txf-tx) * BILINEAR_SHIFT_VALUE );
	const int fv	= int( (tyf-ty) * BILINEAR_SHIFT_VALUE );
	const int fui	= BILINEAR_SHIFT_VALUE - fu;
	const int fvi	= BILINEAR_SHIFT_VALUE - fv;

	const int w1 = (fui*fvi)>>BILINEAR_SHIFT;
	const int w2 = (fui*fv)>>BILINEAR_SHIFT;
	const int w3 = (fu*fvi)>>BILINEAR_SHIFT;
	const int w4 = (fu*fv)>>BILINEAR_SHIFT;

	const int xp1 = tx - (tx+1 >= tex.width) * (tex.width-1) + (tx+1 < tex.width);
	const int yp1 = ty - (ty+1 >= tex.height) * (tex.height-1) + (ty+1 < tex.height);

	const int p1 = tex.data[ty*tex.width + tx];
	const int p2 = tex.data[yp1*tex.width + tx];
	const int p3 = tex.data[ty*tex.width + xp1];
	const int p4 = tex.data[yp1*tex.width + xp1];

	const int r = ((COMPONENT_RED(p1) * w1 + COMPONENT_RED(p2) * w2 + COMPONENT_RED(p3) * w3 + COMPONENT_RED(p4) * w4) >> BILINEAR_SHIFT);
	const int g = ((COMPONENT_GREEN(p1) * w1 + COMPONENT_GREEN(p2) * w2 + COMPONENT_GREEN(p3) * w3 + COMPONENT_GREEN(p4) * w4) >> BILINEAR_SHIFT);
	const int b = ((COMPONENT_BLUE(p1) * w1 + COMPONENT_BLUE(p2) * w2 + COMPONENT_BLUE(p3) * w3 + COMPONENT_BLUE(p4) * w4) >> BILINEAR_SHIFT);

	return r | (g<<GREEN_BIT) | (b<<BLUE_BIT) | ALPHA;

#undef BILINEAR_SHIFT_VALUE
#undef BILINEAR_SHIFT
}

void shader_bilinear_lighting( _fragment& a_in, int& a_out )
{ 
	_material &mat = *(_material_mgr::singleton().get_material(a_in.mtl_id));


	int r = (MIN(COMPONENT_RED(mat.ka), 255) * COMPONENT_RED( mat.kd )) >> 8;
	int g = (MIN(COMPONENT_GREEN(mat.ka), 255) * COMPONENT_GREEN( mat.kd )) >> 8;
	int b = (MIN(COMPONENT_BLUE(mat.ka), 255) * COMPONENT_BLUE( mat.kd )) >> 8;

	_texture &s_texDiffuse = *_texture_mgr::singleton().get_texture(mat.map_kd[0]);
	const int diffuseMap = GetBilinearColor(a_in.cell.u, a_in.cell.v, s_texDiffuse);

	r = (COMPONENT_RED(diffuseMap)*r) >> 8;
	g = (COMPONENT_GREEN(diffuseMap)*g) >> 8;
	b = (COMPONENT_BLUE(diffuseMap)*b) >> 8;

	a_out = COLOR(r, g, b);
}

