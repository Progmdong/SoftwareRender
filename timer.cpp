#include "timer.h"

_timer::_timer():m_fps(0.),m_frames(0),m_time(0.)
{
	QueryPerformanceCounter(&a.count);
	QueryPerformanceFrequency(&a.freq);
	QueryPerformanceCounter(&b.count);
	QueryPerformanceFrequency(&b.freq);
}

void _timer::set_start()
{
	QueryPerformanceCounter(&a.count);
	QueryPerformanceFrequency(&a.freq);
}

void _timer::set_end()
{
	QueryPerformanceCounter(&b.count);
	QueryPerformanceFrequency(&b.freq);
}

const float _timer::get_timef()
{
	return  float(b.count.QuadPart)/b.freq.QuadPart - 
		float(a.count.QuadPart)/a.freq.QuadPart;
}

const double _timer::get_timed()
{
	return  double(b.count.QuadPart)/b.freq.QuadPart - 
		double(a.count.QuadPart)/a.freq.QuadPart;
}

const float _timer::get_fps()
{
	return m_fps;
}

void _timer::elapse()
{
	a = b;
	QueryPerformanceCounter(&b.count);
	QueryPerformanceFrequency(&b.freq);
	m_time += get_timef();
	m_frames++;
	if ( m_time >= 1.f )
	{
		m_fps = m_frames/m_time;
		m_frames = 0;
		m_time = 0.f;
	}
}