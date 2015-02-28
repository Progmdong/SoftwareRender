#ifndef _MATERIAL_MGR_H
#define _MATERIAL_MGR_H
#include "geometry.h"
#include "str.h"
#include "soft_shader.h"
#define MATERIAL_DIFFUSE_MAPS 6

struct _material
{
	_material():ka(0xff000000),kd(0xffffffff),ks(0xffffffff),
		map_bump(0),ns(0)
	{
		for (int i=0; i<6;i++)
		{
			map_kd[i] = 0;
		}
		shader = shader_bilinear_lighting;
	}
	str name;
	int ka;
	int kd;
	int ks;
	int ns;
	int map_kd[MATERIAL_DIFFUSE_MAPS];
	int map_bump;

	void (*shader)(_fragment&, int&);

};

class _material_mgr
{

public:
	static _material_mgr& singleton();
	inline int already_load( std::string str );
	bool load_material_from_file(str file_dir, str mtl_file_name);
	int assign_material( str mtl_name );
	int first_free_slot();
	_material *get_material(int a_mtl_id);

private:
	_material_mgr(){}
	~_material_mgr(){}
	static _material_mgr *m_singleton;

};

#endif
