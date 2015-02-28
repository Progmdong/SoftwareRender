#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <io.h>
#include <Windows.h>
#include <fcntl.h>
#include <iomanip>

#include <iostream>
#include "base_math.h"
#include "framework.h"
#include "core.h"
#include "scene.h"

#pragma  warning( disable : 4996 ) // disable deprecated warning

using namespace std;

void redirect_IO()
{

	static const WORD MAX_CONSOLE_LINES = 500;
	int hConHandle;
	long lStdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;
	AllocConsole();
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),
		&coninfo);
	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),
		coninfo.dwSize);
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );
	ios::sync_with_stdio();

}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPreInstance, LPSTR lpCmdLine, int nShowCmd )
{
	redirect_IO();
	//_framework::create( GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) );
	_framework::create( 512,384 );
	_framework::singleton().init();

	_scene *game = new _scene;
	game->set_framework( &_framework::singleton() );
	game->init();
	_framework::singleton().set_core( game );

	while ( !_framework::singleton().is_exiting() )
	{
		_framework::singleton().update();
	}
}
