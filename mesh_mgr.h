#ifndef _MESH_MGR_H
#define _MESH_MGR_H
#include "str.h"
#include "geometry.h"

class _mesh_mgr
{
public:
	static _mesh_mgr& singleton();
	bool load_mesh( _mesh &a_mesh, str file_dir, str mesh_file_name );
	void rescale_mesh(_mesh& a_mesh, vec3f a_s);
private:
	_mesh_mgr(){}
	~_mesh_mgr(){}
	static _mesh_mgr* m_singleton;
};

#endif