#include "shader_engine.h"
#ifdef SINGLE_THREAD
#include <queue>
#else
#include <boost/lockfree/queue.hpp>
#include <boost/atomic/atomic.hpp>
#include <boost/thread/thread.hpp>
#endif // SINGLE_THREAD

#ifdef SINGLE_THREAD
static std::queue<_fragment_job> s_jobs_stack;
#else

boost::thread_group s_threads;
boost::lockfree::queue<_fragment_job,boost::lockfree::capacity<1024> > s_jobs_stack;
boost::condition_variable s_ready_cond;
boost::mutex s_mtx;
#endif

_shader_engine* _shader_engine::m_singleton = 0;
_shader_engine& _shader_engine::singleton()
{
	if (m_singleton==0)
	{
		m_singleton = new _shader_engine;
	}
	return *m_singleton;
}

#ifdef SINGLE_THREAD

_shader_engine::_shader_engine()
{
}

void _shader_engine::start()
{
	while (!s_jobs_stack.empty())
	{
		_fragment_job job = s_jobs_stack.front();
		job.execute();
		s_jobs_stack.pop();
	}
}

void _shader_engine::add_job(_fragment_job a_job)
{
	s_jobs_stack.push( a_job );
}

void _shader_engine::wait_all_finished()
{
	return;
}

#else

void execute_shading()
{
	_fragment_job job;
	while (true)
	{
		while ( s_jobs_stack.pop(job) )
		{
			job.execute();
		}
		{
			boost::mutex::scoped_lock lock(s_mtx);
			s_ready_cond.wait(lock);
		}
	}


}


_shader_engine::_shader_engine()
{

	for (int i=0; i<NUM_SHADER_THREAD; i++)
	{
		s_threads.create_thread(execute_shading);
	}
}

void _shader_engine::start()
{
	s_ready_cond.notify_all();
}

void _shader_engine::add_job(_fragment_job a_job)
{
	s_jobs_stack.push( a_job );
}

void _shader_engine::wait_all_finished()
{
	while ( !s_jobs_stack.empty() )
	{
	}
}
#endif
