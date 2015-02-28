#ifndef _CORE_
#define _CORE_

class _framework;

class _core
{
public:
	_core(){}
	~_core(){}

	virtual void init()=0;
	virtual void update(const float ) = 0;
	virtual void render() = 0;
	virtual void post_render() = 0;
	virtual void after_render() = 0;
	void set_framework(_framework* a_pfw){m_pfw=a_pfw;}

protected:
	_framework *m_pfw;


};

#endif