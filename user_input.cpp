#include "user_input.h"

_user_input::_user_input()
{
	for (int i=0;i<KEY_COUNT;i++)
	{
		m_state_prev_frame[i] = false;
		m_state_this_frame[i] = false;
	}
}

_user_input* _user_input::m_singleton = 0;
_user_input& _user_input::singleton()
{ 
	if (!m_singleton)
	{
		m_singleton = new _user_input;
	}
	return *m_singleton;
}

void _user_input::set_state( int key, bool state )
{
	m_state_this_frame[key] = state;
}

void _user_input::elapse()
{
	for (int i=0; i<KEY_COUNT; i++)
	{
		m_state_prev_frame[i] = m_state_this_frame[i];
	}
}


bool _user_input::get_key_holding(int a_key)
{
	return (m_state_this_frame[a_key]&&m_state_prev_frame[a_key]);
}

vec2f _user_input::get_delta()
{
	return vec2f( (float)(m_mouse_cursor_prev_frame[0]-m_mouse_cursor_this_frame[0]),
				(float)(m_mouse_cursor_prev_frame[1]-m_mouse_cursor_this_frame[1]) );
}

void _user_input::set_mouse_passive(int a_x, int a_y)
{
	m_mouse_cursor_prev_frame[0] = m_mouse_cursor_this_frame[0];
	m_mouse_cursor_prev_frame[1] = m_mouse_cursor_this_frame[1];
	m_mouse_cursor_this_frame[0] = a_x;
	m_mouse_cursor_this_frame[1] = a_y;
}

void _user_input::set_mouse_left_click(int a_x, int a_y)
{
	m_mouse_left_click_pos[0] = a_x;
	m_mouse_left_click_pos[1] = a_y;
}

void _user_input::set_mouse_right_click(int a_x, int a_y)
{
	m_mouse_right_click_pos[0] = a_x;
	m_mouse_right_click_pos[1] = a_y;
}
