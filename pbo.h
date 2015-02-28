/*  pixel buffer object  */
#ifndef _PBO_H
#define _PBO_H

class PBO
{

public:
	PBO();
	~PBO();

	void init( int a_w, int a_h, int a_d = 4 );
	void cleanup();
	char*	get_buffer_ptr();
	void		upload();
	const unsigned int get_texId(){return m_texId;}
	const int get_width(){return m_width;}
	const int get_height(){return m_height;}

private:
	char* m_data;
	unsigned int m_texId;
	int m_height;
	int m_width;
	int m_depth;
	int m_bytes;

};

#endif