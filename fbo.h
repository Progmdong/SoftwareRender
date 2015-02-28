#ifndef _FBO_H
#define _FBO_H
#include "geometry.h"

struct FBO
{
	FBO();
	~FBO();
	void init( int a_w, int a_h );
	
	void to_shading();
	void copy_texture( char* a_dst );
	void clean_up();
	_texture texture;

	_fragment* dual_buffer[2];

};

#endif