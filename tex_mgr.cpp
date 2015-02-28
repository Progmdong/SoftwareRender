#include "FreeImage/FreeImage.h"
#include "tex_mgr.h"
#include "logger.h"

#define MAX_TEX_SLOT 1024
#define NO_ENOUGH_FREE_TEX_SLOT MAX_TEX_SLOT
#define TEX_NOT_LOADED -1

struct _tex_slot{

	_tex_slot():load_count(0){}

	_texture tex;
	int			load_count;
};

static _tex_slot s_tex[MAX_TEX_SLOT];

_texture_mgr* _texture_mgr::m_singleton = 0;
_texture_mgr& _texture_mgr::singleton()
{
	if (!m_singleton)
	{
		m_singleton = new _texture_mgr;
	}
	return *m_singleton;
}

int _texture_mgr::already_load( str a_s )
{
	for (int i=0; i<MAX_TEX_SLOT;i++)
	{
		if (a_s == s_tex[i].tex.name)
		{
			return i;
		}
	}
	return TEX_NOT_LOADED;
}

_texture *_texture_mgr::get_texture(int a_texId)
{
	return &s_tex[a_texId].tex;
}

int _texture_mgr::first_free_slot()
{
	for (int i=0;i<MAX_TEX_SLOT; i++)
	{
		if(s_tex[i].load_count==0)
			return i;
	}
	return NO_ENOUGH_FREE_TEX_SLOT;
}

int _texture_mgr::allocate_texture( std::string tex_file_name )
{
	int texId = already_load(tex_file_name);

	if (texId != TEX_NOT_LOADED)
	{
		s_tex[ texId ].load_count++;
		return texId;
	}

	texId = first_free_slot();
	if ( texId==NO_ENOUGH_FREE_TEX_SLOT  )
	{
		LOG( "no enough preallocate slots to load new texture." );
		return -1;
	}

	if( !load_from_file( s_tex[texId].tex, tex_file_name) )
	{
		LOG( "read texture file failed: "+tex_file_name );
		return -1;
	}

	s_tex[texId].load_count++;
	return texId;
}

bool _texture_mgr::load_from_file( _texture &a_tex, std::string tex_file_name )
{
	FILE *tex_file = fopen( tex_file_name.c_str(), "r" );
	if(!tex_file)
	{
		return false;
	}

	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	fif = FreeImage_GetFileType( tex_file_name.c_str(), 0 );
	if (fif == FIF_UNKNOWN) fif = FreeImage_GetFIFFromFilename( tex_file_name.c_str() );
	FIBITMAP* tmp =  FreeImage_Load( fif, tex_file_name.c_str() );
	FIBITMAP* fbm = FreeImage_ConvertTo32Bits(tmp );
	FreeImage_Unload(tmp);
	unsigned char* bits = FreeImage_GetBits( fbm );

	a_tex.width = FreeImage_GetWidth( fbm );
	a_tex.height = FreeImage_GetHeight( fbm );
	a_tex.pixels = a_tex.width * a_tex.height;
	a_tex.data = new int[ a_tex.pixels ];
	a_tex.name = tex_file_name;
	const int* data = (int*)bits;

	for(int i=0; i<a_tex.pixels; ++i)
	{
		int r = (data[i] & 0x00ff0000) >> 16;
		int g = (data[i] & 0x0000ff00) >> 8;
		int b = (data[i] & 0x000000ff);
		a_tex.data[i] = r | (g << 8) | (b << 16) | 0xff000000;
	}

	FreeImage_Unload( fbm );
	return true;
}

#undef  MAX_TEX_SLOT
#undef  NO_ENOUGH_FREE_TEX_SLOT
#undef  TEX_NOT_LOADED 
