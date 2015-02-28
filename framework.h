#ifndef _FRAMEWORK_H
#define _FRAMEWORK_H
#include <windows.h>
#include "core.h"
#include "timer.h"

class _framework
{

public:
	static void create( const int a_sx, const int a_sy );
	static _framework& singleton();
	void init();
	void update();
	bool is_exiting(){return m_exiting;}
	void set_core( _core* core );
	_core* get_core(){return m_core;};
	void set_active( bool a_s  ){m_active=a_s;};
	bool is_active(){return m_active;}
	void setx(const int x){ m_x = x; }
	void sety(const int y){ m_y = y; }
	const int getx(){ return m_x; }
	const int gety(){ return m_y; }

	static void control_input( UINT uMsg, WPARAM wParam, LPARAM lParam );
	static LRESULT CALLBACK callback( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	static void setup_pixel_format(HDC hdc);
	void create_window();
private:
	_framework( const int a_x, const int a_y );
	~_framework();

	static _framework *m_singleton;
	int m_x;
	int m_y;
	bool m_active;
	bool m_exiting;
	HWND m_hwnd;
	HDC m_hdc;
	
	_core *m_core;
	_timer m_timer;
};

#endif