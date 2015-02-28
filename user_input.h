#ifndef _USER_INPUT_H
#define _USER_INPUT_H

#include "base_math.h"

enum
{
	SPECIAL_KEY_START = 256,
	MOUSE_LBUTTON,
	MOUSE_RBUTTON,
	MOUSE_CURSOR_X,
	MOUSE_CURSOR_Y,
	KEY_COUNT
};

class _user_input
{
public:

	static _user_input& singleton();
	void set_state( int key, bool state );
	void elapse();
	void set_mouse_left_click(int a_x, int a_y);
	void set_mouse_right_click(int a_x, int a_y);
	void set_mouse_passive(int a_x, int a_y);

	bool get_key_holding(int a_key);
	vec2f get_delta();

private:
	_user_input();
	~_user_input();
	static _user_input *m_singleton;
	bool m_state_this_frame[KEY_COUNT];
	bool m_state_prev_frame[KEY_COUNT];

	int m_mouse_cursor_this_frame[2];
	int m_mouse_cursor_prev_frame[2];

	int m_mouse_left_click_pos[2];
	int m_mouse_right_click_pos[2];
};

#endif