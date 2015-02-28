#ifndef _TEX_MGR_H
#define _TEX_MGR_H
#include <string>
#include <hash_set>
#include "geometry.h"

class _texture_mgr
{
public:
	static _texture_mgr& singleton();
	_texture *get_texture(int a_texId);
	int allocate_texture( std::string tex_file_name );

private:

	inline int already_load( str a_s );
	bool	load_from_file( _texture &a_tex, std::string tex_file_name );
	inline int first_free_slot();

	static _texture_mgr* m_singleton;
};

#endif