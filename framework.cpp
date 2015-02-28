#include <windows.h>
#include <stdio.h>
#include "framework.h"
#include "user_input.h"
#include "base_math.h"
#include "gl/glew.h"
#include "rasterizer.h"

_framework::_framework( const int a_x, const int a_y )
	:m_x(a_x), m_y(a_y),m_exiting(false),m_active(true),m_core(NULL)
{

}

_framework::~_framework()
{

}

void _framework::create( const int a_sx, const int a_sy )
{
	if ( m_singleton )
	{
		delete m_singleton;
		m_singleton = nullptr;
	}
	m_singleton = new _framework(a_sx, a_sy);
}

_framework* _framework::m_singleton = 0;
_framework& _framework::singleton()
{
	return *m_singleton;
}

void _framework::control_input( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	int fwKeys = (int)wParam;
	LPARAM keyData;

	switch (uMsg)
	{
	case WM_CREATE:	// window creation
		break;
	case WM_DESTROY:            // window destroy
	case WM_QUIT:
	case WM_CLOSE:			//alt-f4;
		PostQuitMessage(0);
		break;

	case WM_ACTIVATEAPP:        // activate app
		break;
	case WM_LBUTTONDOWN:
		_user_input::singleton().set_state(MOUSE_LBUTTON, true);
		_user_input::singleton().set_mouse_left_click( LOWORD(lParam), HIWORD(lParam) );
		break;
	case WM_RBUTTONDOWN:
		_user_input::singleton().set_state(MOUSE_RBUTTON, true);
		_user_input::singleton().set_mouse_right_click( LOWORD(lParam), HIWORD(lParam) );
		break;
	case WM_MOUSEMOVE:
		_user_input::singleton().set_mouse_passive(LOWORD(lParam),HIWORD(lParam));
		break;
	case WM_LBUTTONUP:
		_user_input::singleton().set_state(MOUSE_LBUTTON, false);
		break;
	case WM_RBUTTONUP:
		_user_input::singleton().set_state(MOUSE_RBUTTON, false);
		break;
	case WM_KEYDOWN:
		_user_input::singleton().set_state( fwKeys, true );
		break;
	case WM_KEYUP:
		_user_input::singleton().set_state(fwKeys, false);
	case WM_ACTIVATE:
		if ( wParam != 0 )
			singleton().set_active( true );
		else
			singleton().set_active(  false );
		break;
	default:
		break;
	}
}

void _framework::setup_pixel_format(HDC hdc)
{
	int pixelFormat;

	PIXELFORMATDESCRIPTOR pfd =
	{   
		sizeof(PIXELFORMATDESCRIPTOR),  // size
		1,                          // version
		PFD_SUPPORT_OPENGL |        // OpenGL window
		PFD_DRAW_TO_WINDOW |        // render to window
		PFD_DOUBLEBUFFER,           // support double-buffering
		PFD_TYPE_RGBA,              // color type
		32,                         // prefer color depth
		0, 0, 0, 0, 0, 0,           // color bits (ignored)
		0,                          // no alpha buffer
		0,                          // alpha bits (ignored)
		0,                          // no accumulation buffer
		0, 0, 0, 0,                 // accumulate bits (ignored)
		16,                         // depth buffer
		0,                          // no stencil buffer
		0,                          // no auxiliary buffers
		PFD_MAIN_PLANE,             // main layer
		0,                          // reserved
		0, 0, 0,                    // no layer, visible, damage masks
	};

	pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd);
}

LRESULT CALLBACK _framework::callback( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	control_input(uMsg, wParam, lParam);
	static HDC hdc;
	static HGLRC hrc;
	int height, width;
	mat4x4f proj_mat;
	PAINTSTRUCT ps;

	switch (uMsg)
	{
	case  WM_CREATE:
		hdc = GetDC(hWnd);
		setup_pixel_format(hdc);
		hrc = wglCreateContext(hdc);
		wglMakeCurrent(hdc, hrc);
		break;
	case WM_CLOSE:
		wglMakeCurrent(hdc, NULL);
		wglDeleteContext(hrc);
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		height = HIWORD(lParam);
		width = LOWORD(lParam);
		singleton().setx(width);
		singleton().sety(height);
		if ( height==0 )
			height=1;

		glViewport(0,0, width, height);
		glMatrixMode(GL_PROJECTION);

		proj_mat.projection( 45.f,float(width)/float(height), 1.f, 1000.f );
		glLoadMatrixf( proj_mat.cell );
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		break;
	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
	case WM_QUIT:
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void _framework::create_window()
{
	WNDCLASSEX windowClass;
	DWORD dwStyle;
	RECT windowRect;

	windowRect.left = (long)0;
	windowRect.right = (long)m_x;
	windowRect.top = (long)0;
	windowRect.bottom = (long)m_y;

	//fill out the window class structure
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style           = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc    =  callback;
	windowClass.cbClsExtra      = 0;
	windowClass.cbWndExtra      = 0;
	windowClass.hInstance       = 0;
	windowClass.hIcon           = LoadIcon(NULL, IDI_APPLICATION);  // default icon
	windowClass.hCursor         = LoadCursor(NULL, IDC_ARROW);      // default arrow
	windowClass.hbrBackground   = NULL;                             // don't need background
	windowClass.lpszMenuName    = NULL;                             // no menu
	windowClass.lpszClassName   = "GLClass";
	windowClass.hIconSm         = LoadIcon(NULL, IDI_WINLOGO);      // windows logo small icon

	// register the windows class
	if (!RegisterClassEx(&windowClass))
		return;

	dwStyle = 0;
	DWORD dwWidth = (GetSystemMetrics(SM_CXSCREEN)>>1) - (windowRect.right>>1);
	DWORD dwHeight = (GetSystemMetrics(SM_CYSCREEN)>>1) - (windowRect.bottom>>1);

	m_hwnd = CreateWindow(
		"GLClass",
		"SoftRasterizer",
		dwStyle,
		dwWidth,
		dwHeight,
		windowRect.right,
		windowRect.bottom,
		NULL,
		NULL,
		NULL,
		NULL );

	m_hdc = GetDC(m_hwnd);
	// check if window creation failed (hwnd would equal NULL)
	if (!m_hwnd)
	{
		MessageBox(NULL, "Error Creating OpenGL Window", NULL, MB_OK);
		return;
	}
	//END WINDOW INITIATION

	ShowWindow(m_hwnd, SW_SHOWDEFAULT);
	UpdateWindow(m_hwnd);

}

void _framework::init()
{
	create_window();

	// check if window creation failed (hwnd would equal NULL)
	if (!m_hwnd)
	{
		MessageBox(NULL, "Error Creating OpenGL Window", NULL, MB_OK);
		return;
	}
	//END WINDOW INITIATION

	ShowWindow(m_hwnd, SW_SHOWDEFAULT);
	UpdateWindow(m_hwnd);

	m_timer.set_start();
	m_timer.set_end();

	tagRECT rectPos;
	tagRECT rectSize;
	GetWindowRect(m_hwnd, &rectPos);
	GetClientRect(m_hwnd, &rectSize);

	int x = (rectSize.right>>1);
	int y = (rectSize.bottom>>1);

	SetCursorPos(rectPos.left + x, rectPos.top + y);

}

void _framework::update()
{
	static char s_info[128];
	
	if ( m_active )
	{
		m_timer.elapse();
		_rasterizer::singleton().clear_polygon_draw_counter();
		if ( m_core )
		{
			m_core->update(m_timer.get_timef());
			m_core->post_render();
			m_core->render();
			m_core->after_render();
		}

		sprintf(s_info,"FPS:%2.2f, POLYGON_DRAWED:%d",m_timer.get_fps(), _rasterizer::singleton().get_polygon_drawed());
		SetWindowTextA(m_hwnd, s_info);
		SwapBuffers(m_hdc);
		_user_input::singleton().elapse();

	}
	MSG msg;
	while (PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE ))
	{
		if (!GetMessage (&msg, NULL, 0, 0))
		{
			m_exiting = true;
			return;
		}
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

void _framework::set_core( _core* core )
{
	m_core = core;
}

