#include "fbo.h"
#include "shader_engine.h"

FBO::FBO()
{
	dual_buffer[0] = 0;
	dual_buffer[1] = 0;
}

FBO::~FBO()
{
	clean_up();
}

void FBO::init( int a_w, int a_h )
{
	clean_up();
	texture.width = a_w;
	texture.height = a_h;
	texture.pixels = texture.width * texture.height;

	dual_buffer[0] = new _fragment[texture.pixels];
	dual_buffer[1] = new _fragment[texture.pixels];
	memset( dual_buffer[0], 0, sizeof(_fragment)*texture.pixels );
	memset( dual_buffer[1], 0, sizeof(_fragment)*texture.pixels );

	texture.data = new int[texture.pixels];
}

void FBO::clean_up()
{
	if ( dual_buffer[0] )delete [] dual_buffer[0];
	if ( dual_buffer[1] )delete [] dual_buffer[1];
	if ( texture.data )delete texture.data;

	texture.width = 0;
	texture.height = 0;
	texture.pixels = 0;
}

void FBO::to_shading()
{
	_shader_engine::singleton().wait_all_finished();
	memcpy( dual_buffer[1], dual_buffer[0], texture.pixels*sizeof(_fragment) );
}

void FBO::copy_texture( char* a_dst )
{
	for ( int y=0; y<texture.height; y++ )
	{
		memcpy(&a_dst[y*sizeof(int)*texture.width],
			&texture.data[y*texture.width], sizeof(int)*texture.width);
	}
}