#include "mesh_mgr.h"
#include "logger.h"
#include "material_mgr.h"
#include <assert.h>

_mesh_mgr* _mesh_mgr::m_singleton = 0;
_mesh_mgr& _mesh_mgr::singleton()
{
	if (!m_singleton)
	{
		m_singleton = new _mesh_mgr;
	}
	return *m_singleton;
}

bool _mesh_mgr::load_mesh( _mesh &a_mesh,str file_dir, str mesh_file_name )
{
	LOG("load mesh file: "+mesh_file_name);
	FILE *mesh_file = fopen( (file_dir+mesh_file_name).c_str(), "r" );
	if ( !mesh_file )
		return false;

	char line[1024];
	int idx[12];

	while ( !feof(mesh_file) )
	{
		fgets( line, sizeof(line), mesh_file );
		str_format(line);

		if ( line[0]=='V' && line[1]==' ' )
		{
			a_mesh.vc++;
			continue;
		}
		else if ( line[0]=='V' && line[1]=='N' )
		{
			a_mesh.nc++;
			continue;
		}
		else if( line[0]=='V' && line[1]=='T' )
		{
			a_mesh.uvc++;
			continue;
		}

		else if( sscanf(line,"F %i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i",
			idx,idx,idx,
			idx,idx,idx,
			idx,idx,idx,
			idx,idx,idx)==12 )
		{
			a_mesh.fc += 2;
			continue;
		}

		else if( sscanf(line,"F %i/%i/%i %i/%i/%i %i/%i/%i",
			idx,idx,idx,
			idx,idx,idx,
			idx,idx,idx)==9 )
		{
			a_mesh.fc ++;
			continue;
		}

		else if(line[0] == 'U' && line[1] == 'S')
		{
			a_mesh.sc++;
			continue;
		}
	}

	if ( a_mesh.vc==0 )
	{
		fclose(mesh_file);
		return false;
	}

	rewind(mesh_file);
	if ( a_mesh.sc==0 )a_mesh.sc++;

	if ( a_mesh.sc )
	{
		a_mesh.seg = new _segment*[ a_mesh.sc ];	
		for ( int s=0;s<a_mesh.sc; s++ )
		{
			a_mesh.seg[s] = new _segment;
		}
	}
	if ( a_mesh.fc )a_mesh.face = new _face[a_mesh.fc];
	if ( a_mesh.vc )a_mesh.vertex = new vec3f[a_mesh.vc];
	if ( a_mesh.nc )a_mesh.normal = new vec3f[a_mesh.nc];
	if ( a_mesh.uvc )a_mesh.uv = new vec2f[a_mesh.uvc];

	float v[3];
	char txt[256];
	int vi,nvi,uvi,fi,si;
	vi=nvi=uvi=si=fi=0;

	a_mesh.seg[0]->face_start = 0;

	while ( !feof(mesh_file) )
	{
		fgets( line, sizeof(line), mesh_file );
		str_format( line );

		if ( sscanf(line, "V %f %f %f", v, v+1, v+2)==3 )
		{
			assert(vi < a_mesh.vc);
			a_mesh.vertex[ vi ].x = v[0];
			a_mesh.vertex[ vi ].y = v[1];
			a_mesh.vertex[ vi ].z = v[2];
			vi++;
			continue;
		}
		else if ( sscanf(line, "VN %f %f %f", v, v+1, v+2)==3 )
		{
			assert(nvi < a_mesh.nc);
			a_mesh.normal[ nvi ].x = v[0];
			a_mesh.normal[ nvi ].y = v[1];
			a_mesh.normal[ nvi ].z = v[2];
			nvi++;
			continue;
		}
		else if ( sscanf(line, "VT %f %f", v, v+1)==2 )
		{
			assert(uvi < a_mesh.uvc);
			a_mesh.uv[ uvi ].u = v[0];
			a_mesh.uv[ uvi ].v = v[1];
			uvi++;
			continue;
		}

		else if( sscanf(line,"F %i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i",
			idx,idx+1,idx+2,
			idx+3,idx+4,idx+5,
			idx+6,idx+7,idx+8,
			idx+9,idx+10,idx+11)==12 )
		{
			//double triangles face
			_face& face1 = a_mesh.face[fi++];
			_face& face2 = a_mesh.face[fi++];

			for ( int i=0; i<3; i++ )
			{
				face1.v[i] = idx[ i*3] -1;
				face1.uv[i] = idx[i*3+1] -1;
				face1.nv[i] = idx[i*3+2] -1;
			}

			face2.v[0] = idx[0]-1;
			face2.uv[0] = idx[1]-1;
			face2.nv[0] = idx[2]-1;

			face2.v[1] = idx[6]-1;
			face2.uv[1] = idx[7]-1;
			face2.nv[1] = idx[8]-1;

			face2.v[2] = idx[9]-1;
			face2.uv[2] = idx[10]-1;
			face2.nv[2] = idx[11]-1;

			continue;
		}

		else if( sscanf(line,"F %i/%i/%i %i/%i/%i %i/%i/%i",
			idx,idx+1,idx+2,
			idx+3,idx+4,idx+5,
			idx+6,idx+7,idx+8)==9 )
		{
			assert(fi < a_mesh.fc);
			_face& face = a_mesh.face[fi];
			//single triangle face
			for ( int i=0; i<3; i++ )
			{
				face.v[i] = idx[ i*3] -1;
				face.uv[i] = idx[i*3+1] -1;
				face.nv[i] = idx[i*3+2] -1;
			}
			fi++;
			continue;

		}

		else if ( sscanf(line, "USEMTL %s",txt)==1 )
		{
			a_mesh.seg[si]->mtl_name = txt;
			if ( si>0 )
			{
				a_mesh.seg[si]->face_start = fi;
				a_mesh.seg[si-1]->face_end = fi;
			}
			si++;
			continue;
		}
		else if ( sscanf(line,"MTLLIB %s",txt)==1 )
		{
			str mtl_dir = "data/materials/"+mesh_file_name+"/";
			_material_mgr::singleton().load_material_from_file( mtl_dir , txt );
			continue;
		}

	}
	fclose(mesh_file);
	a_mesh.seg[si-1]->face_end = fi;

	for ( int s=0; s<a_mesh.sc; s++ )
	{
		_segment &seg = *a_mesh.seg[s];
		seg.mtlId = _material_mgr::singleton().assign_material( seg.mtl_name );

		//create box
		vec3f v_min = vec3f(1.,1.,1.)*99999;
		vec3f v_max = vec3f(1.,1.,1.)*-99999;

		for ( int f=seg.face_start; f<seg.face_end;++f )
		{
			for ( int v=0; v<3; v++ )
			{
				vec3f& t_vec = a_mesh.vertex[ a_mesh.face[f].v[v] ];
				v_min = v_min.min_vec(t_vec);
				v_max = v_max.max_vec(t_vec);
			}
		}

		vec3f *pv[2] = { &v_min, &v_max };

		/*
			 6______7
			 /|    /|
		   2/_|___/3|
			|4|___|_|5
			|/	  | /
		    |_____|/
			0      1
		*/
		for ( int i=0; i<8;i++ )
		{
			int first = i & 0x1;
			int  second = (i & 0x2)>>1;
			int third  = (i & 0x4)>>2;

			seg.box_local[i] = vec3f( pv[first]->x, pv[second]->y, pv[third]->z );
		}
		
		seg.box_local_center = (v_min + v_max) * 0.5;
		seg.diag = (v_max - v_min).length();

	}

	LOG( "load mesh successfully." );
}

void _mesh_mgr::rescale_mesh(_mesh& a_mesh, vec3f a_s)
{
	vec3f v;
	for ( int i=0; i<a_mesh.vc; ++i )
	{
		a_mesh.vertex[i] = a_mesh.vertex[i]*vec3f(a_s.x, a_s.y, a_s.z);
	}
	vec3f vec;
	vec3f v_min = vec3f(1,1,1) * 999999;
	vec3f v_max = vec3f(1,1,1) * -999999;
	for (int s=0; s<a_mesh.sc; s++)
	{
		for(int f=a_mesh.seg[s]->face_start; f<a_mesh.seg[s]->face_end; ++f)
		{
			for(int v=0; v<3; ++v)
			{
				vec = a_mesh.vertex[a_mesh.face[f].v[v]];
				v_min = v_min.min_vec(vec);
				v_max = v_max.max_vec(vec);
			}
		}

		a_mesh.seg[s]->box_local[0] = vec3f(v_min.x, v_max.y, v_min.z);
		a_mesh.seg[s]->box_local[1] = vec3f(v_max.x, v_max.y, v_min.z);
		a_mesh.seg[s]->box_local[2] = vec3f(v_max.x, v_max.y, v_max.z);
		a_mesh.seg[s]->box_local[3] = vec3f(v_min.x, v_max.y, v_max.z);

		a_mesh.seg[s]->box_local[4] = vec3f(v_min.x, v_min.y, v_min.z);
		a_mesh.seg[s]->box_local[5] = vec3f(v_max.x, v_min.y, v_min.z);
		a_mesh.seg[s]->box_local[6] = vec3f(v_max.x, v_min.y, v_max.z);
		a_mesh.seg[s]->box_local[7] = vec3f(v_min.x, v_min.y, v_max.z);
		a_mesh.seg[s]->box_local_center = (v_min + v_max) * 0.5f;

		v_max = v_max - v_min;
		a_mesh.seg[s]->diag = v_max.length();
	}

}