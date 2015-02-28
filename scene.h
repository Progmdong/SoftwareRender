#ifndef _GAME_H
#define _GAME_H

#include "core.h"
#include "fbo.h"
#include "rasterizer.h"
#include "pbo.h"

class _framework;

class _scene : public _core
{
public:
	_scene(){};
	~_scene(){};


	void init();
	void update(const float);
	void render();
	void post_render();
	void after_render();
	void load_scene();
	void start_shading();

private:
	FBO m_fbo;
	PBO m_pbo;
};

#endif