#ifndef _TIMER_H
#define _TIMER_H

#include <windows.h>

class _timer
{
public:
	_timer();
	void set_start();
	void set_end();
	const float get_timef();
	const double get_timed();
	const float get_fps();
	void elapse();

private:
	struct querydata{
		LARGE_INTEGER count;
		LARGE_INTEGER freq;
	};

	querydata a;
	querydata b;
	float m_fps;
	float m_time;
	int m_frames;
};

#endif