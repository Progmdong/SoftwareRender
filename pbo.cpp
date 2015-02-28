#include "pbo.h"
#include "gl/glew.h"
#include <memory>

PBO::PBO():m_data(0),m_texId(0),
	m_height(0),m_width(0),m_bytes(0)
{
};

void PBO::init( int a_w, int a_h, int a_d /* = 4 */ )
{
	m_depth = a_d;
	m_height = a_h;
	m_width = a_w;
	m_bytes = m_width * m_height * m_depth;

	glGenTextures(1,&m_texId);
	glBindTexture(GL_TEXTURE_2D, m_texId);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,	GL_NEAREST );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

	m_data = new char[m_bytes];
	memset(m_data, 0, m_bytes);

	if(m_depth == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	}else{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

PBO::~PBO()
{
	cleanup();
}

void PBO::cleanup()
{
	if ( m_texId )
	{
		glDeleteTextures(1,&(m_texId));
		m_texId = 0;
	}
	m_bytes = 0;
	m_height = 0;
	m_width = 0;
	m_depth = 0;
	if ( m_data )
	{
		delete m_data;
	}
	m_data = 0;
}

char* PBO::get_buffer_ptr()
{
	return m_data;
}

void PBO::upload()
{
	glBindTexture(GL_TEXTURE_2D, m_texId);
	if(m_depth == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data);
	}
	else{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_data);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

