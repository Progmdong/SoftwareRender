#include "fragment_job.h"
#include <assert.h>

_fragment_job::_fragment_job()
	:render_target(NULL)
{

}


_fragment_job::_fragment_job( int a_y1, int a_y2, FBO *a_fbo )
	:y1(a_y1),y2(a_y2),render_target(a_fbo)
{
}

//_fragment_job& _fragment_job::operator=(_fragment_job& a_f)
//{
//	this->y1 = a_f.y1;
//	this->y2 = a_f.y2;
//	this->render_target = a_f.render_target;
//	return *this;
//}

void _fragment_job::execute()
{
	for (int y=y1;y<y2;y++)
	{
		for (int x=0; x<render_target->texture.width; x++)
		{
			int i = y*render_target->texture.width + x;
			_fragment &frag = render_target->dual_buffer[1][i];

			if ( frag.shader==0 )
			{
				render_target->texture.data[i] = 0xffffffff;
				continue;
			}

			//texture correction
			const float zcorr = 1.f / frag.cell.z;
			frag.cell.u *= zcorr;
			frag.cell.v *= zcorr;

			//normalize normal
			const float nl = 1.f / sqrt( frag.cell.nx*frag.cell.nx +
									frag.cell.ny*frag.cell.ny +
									frag.cell.nz*frag.cell.nz);
			frag.cell.nx *= nl;
			frag.cell.ny *= nl;
			frag.cell.nz *= nl;

			frag.shader(frag, render_target->texture.data[i]);
		}
	}
}