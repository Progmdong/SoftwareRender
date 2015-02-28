#include"material_mgr.h"
#include "logger.h"
#include "tex_mgr.h"
#include "soft_shader.h"
#define MAX_MTL_SLOT 1024
#define NO_ENOUGH_FREE_MTL_SLOT MAX_MTL_SLOT
#define MTL_NOT_LOADED -1

struct _mtl_slot
{
	_mtl_slot():load_count(0){}
	_material mtl;
	int		load_count;
};



static _mtl_slot s_mtl[ MAX_MTL_SLOT ];


_material_mgr* _material_mgr::m_singleton = 0;
_material_mgr& _material_mgr::singleton()
{
	if (!m_singleton)
	{
		m_singleton = new _material_mgr;
	}
	return *m_singleton;
}

_material *_material_mgr::get_material(int a_mtl_id)
{
	return &(s_mtl[a_mtl_id].mtl);
}

int _material_mgr::already_load( std::string str )
{
	for (int i=0; i<MAX_MTL_SLOT;i++)
	{
		if (s_mtl[i].mtl.name == str)
		{
			return i;
		}
	}
	return MTL_NOT_LOADED;
}

int _material_mgr::first_free_slot()
{
	for (int i=0;i<MAX_MTL_SLOT; i++)
	{
		if(s_mtl[i].load_count==0 && s_mtl[i].mtl.name.empty())
			return i;
	}
	return NO_ENOUGH_FREE_MTL_SLOT;
}

int _material_mgr::assign_material( str mtl_name )
{
	int mtlId = already_load( mtl_name );
	if ( mtlId != MTL_NOT_LOADED )
	{
		s_mtl[mtlId].load_count++;
		return mtlId;
	}
	return MTL_NOT_LOADED;

}

bool _material_mgr::load_material_from_file(str file_dir,str mtl_file_name)
{
	str file_path = file_dir + mtl_file_name;
	FILE* file = fopen( file_path.c_str(), "r");
	if (file==0)
	{
		return false;
	}

	char line[256];
	char txt[256];
	float f[3];
	int	  i[3];
	int mtlId;

	bool find_next_mtl = false;
	while (!feof(file))
	{
		fgets(line, sizeof(line), file);
		str_format(line);
		if ( find_next_mtl==true )
		{
			if ( sscanf(line, "NEWMTL %s", txt)!=1 )
			{
				continue;
			}
			find_next_mtl = false;
				
		}
		if ( sscanf(line,"NEWMTL %s",txt)==1 )
		{
			str name = txt;
			mtlId = already_load(name);
			if ( mtlId!=MTL_NOT_LOADED )
			{
				find_next_mtl = true;
				//skip current material input
				continue;
			}
			mtlId = first_free_slot();
			if ( mtlId == NO_ENOUGH_FREE_MTL_SLOT )
			{
				LOG("No enough free material slot");
				return false;
			}
			s_mtl[mtlId].mtl.name = txt;
			continue;
		}
		else if ( sscanf(line, "KA %f %f %f",&f[0],&f[1],&f[2])==3 )
		{
			s_mtl[mtlId].mtl.ka = COLOR_CLAMP(int(f[0]*255),int(f[1]*255),int(f[2]*255));
			continue;
		}
		else if ( sscanf(line, "KD %f %f %f",&f[0], &f[1], &f[2])==3 )
		{
			s_mtl[mtlId].mtl.kd = COLOR_CLAMP(int(f[0]*255),int(f[1]*255),int(f[2]*255));
			continue;
		}
		else if ( sscanf(line, "KS %f %f %f",&f[0], &f[1], &f[2])==3 )
		{
			s_mtl[mtlId].mtl.ks = COLOR_CLAMP(int(f[0]*255),int(f[1]*255),int(f[2]*255));
			continue;
		}
		else if ( sscanf(line, "NS %f",&f[0])==1 )
		{
			s_mtl[mtlId].mtl.ns = int(f[0]);
			continue;
		}
		else if ( sscanf(line, "MAP_KD %s",txt)==1 )
		{
			int texId = _texture_mgr::singleton().allocate_texture(file_dir+txt);
			if ( texId>0 )
			{
				s_mtl[mtlId].mtl.map_kd[0] = texId;
			}
			continue;
		}
		else if ( sscanf(line, "MAP_1 %s", txt)==1 )
		{
			int texId = _texture_mgr::singleton().allocate_texture(file_dir+txt);
			if ( texId>0 )
			{
				s_mtl[mtlId].mtl.map_kd[1] = texId;
			}
			continue;
		}
		else if ( sscanf(line, "MAP_2 %s", txt)==1 )
		{
			int texId = _texture_mgr::singleton().allocate_texture(file_dir+txt);
			if ( texId>0 )
			{
				s_mtl[mtlId].mtl.map_kd[2] = texId;
			}
			continue;
		}
		else if ( sscanf(line, "MAP_3 %s", txt)==1 )
		{
			int texId = _texture_mgr::singleton().allocate_texture(file_dir+txt);
			if ( texId>0 )
			{
				s_mtl[mtlId].mtl.map_kd[3] = texId;
			}
			continue;
		}
		else if ( sscanf(line, "MAP_4 %s", txt)==1 )
		{
			int texId = _texture_mgr::singleton().allocate_texture(file_dir+txt);
			if ( texId>0 )
			{
				s_mtl[mtlId].mtl.map_kd[4] = texId;
			}
			continue;
		}
		else if ( sscanf(line, "MAP_5 %s", txt)==1 )
		{
			int texId = _texture_mgr::singleton().allocate_texture(file_dir+txt);
			if ( texId>0 )
			{
				s_mtl[mtlId].mtl.map_kd[5] = texId;
			}
			continue;
		}
		else if ( sscanf(line, "MAP_BUMP %s",txt)==1 )
		{
			int texId = _texture_mgr::singleton().allocate_texture(file_dir+txt);
			if ( texId>0 )
			{
				s_mtl[mtlId].mtl.map_bump = texId;
			}
			continue;
		}
		else if ( sscanf(line, "SHADER %i",&i[0])==1 )
		{
			s_mtl[mtlId].mtl.shader = shader_bilinear_lighting;
			continue;
		}
	}
	fclose(file);
	return true;
}


#undef MAX_MTL_SLOT
#undef NO_ENOUGH_FREE_MTL_SLOT
#undef MTL_NOT_LOADED