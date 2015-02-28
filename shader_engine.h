#ifndef _SHADER_ENGINE_H
#define _SHADER_ENGINE_H

//#define SINGLE_THREAD

#include "fragment_job.h"

#define NUM_SHADER_THREAD 4


class _shader_engine
{
public:
	static _shader_engine& singleton();
	void start();
	void add_job(_fragment_job a_job);
	void wait_all_finished();

private:
	_shader_engine();
	~_shader_engine();
	static _shader_engine* m_singleton;

	


};

#endif