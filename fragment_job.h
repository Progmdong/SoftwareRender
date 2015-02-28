#ifndef _FRAGMENT_JOB_H
#define _FRAGMENT_JOB_H
#include "fbo.h"
struct _fragment_job
{
	_fragment_job();
	_fragment_job( int a_y1, int a_y2, FBO *a_fbo );
	//_fragment_job& operator=(_fragment_job& a_f);
	void execute();

	FBO *render_target;
	int y1;
	int y2;
};

#endif