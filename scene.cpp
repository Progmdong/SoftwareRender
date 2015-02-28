#include "scene.h"
#include "rasterizer.h"
#include "dg.h"
#include "framework.h"
#include "mesh_mgr.h"
#include "logger.h"
#include "user_input.h"
#include "gl/glew.h"
#include "shader_engine.h"
#include "fragment_job.h"

static vec3f s_camera_pos;
static vec3f s_camera_axis;


void _scene::init()
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int buffw = viewport[2];
	int buffh = viewport[3];

	_rasterizer::singleton().init(buffw,buffh);
	m_fbo.init( buffw, buffh );
	m_pbo.init(buffw,buffh);
	_rasterizer::singleton().set_target( m_fbo );

	load_scene();


}

void _scene::update(const float a_dt)
{
	mat4x4f camera_mat;
#define UI_HOLDING(x)  (_user_input::singleton().get_key_holding(x))
	if ( UI_HOLDING(MOUSE_LBUTTON) )
	{
		float ratio = float(m_pfw->getx()) / m_pfw->gety();
		vec2f vd = _user_input::singleton().get_delta();
		s_camera_axis.x += -vd.y * a_dt * 5 * ratio;
		s_camera_axis.y += -vd.x * a_dt * 5 * ratio;
	}
	camera_mat.rotate_x(-s_camera_axis.x);
	camera_mat.rotate_y(-s_camera_axis.y);

	if ( UI_HOLDING('W') )
	{				
		s_camera_pos = s_camera_pos + camera_mat.transform_rotation(vec3f(0,0,1)) * a_dt * 500;
	}
	if ( UI_HOLDING('S') )
	{
		s_camera_pos = s_camera_pos - camera_mat.transform_rotation(vec3f(0,0,1)) * a_dt * 500;
	}
	if ( UI_HOLDING('A') )
	{
		s_camera_pos = s_camera_pos + camera_mat.transform_rotation(vec3f(1,0,0)) * a_dt * 500;
	}
	if ( UI_HOLDING('D') )
	{
		s_camera_pos = s_camera_pos - camera_mat.transform_rotation(vec3f(1,0,0)) * a_dt * 500;
	}
	camera_mat.set_translation(s_camera_pos);
	_rasterizer::singleton().set_target(m_fbo);
	camera_mat.invert();
	_dgraph::singleton().set_camera_matrix( camera_mat );
	_dgraph::singleton().render();

	/*	use shader engine */

	_shader_engine::singleton().wait_all_finished();
	m_fbo.to_shading();
	m_fbo.copy_texture( m_pbo.get_buffer_ptr() );
	start_shading();

	m_pbo.upload();

#undef UI_HOLDING
}

void _scene::post_render()
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1, 1, 1, -1, 0, 1);
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib( GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );
}

void _scene::render()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_pbo.get_texId());
	glBegin(GL_QUADS);
	{
		glTexCoord2f( 1.0f, 0.0f ); glVertex3f( -1.0f, 1.0f, 0.0f );
		glTexCoord2f( 0.0f, 0.0f );	glVertex3f( 1.0f, 1.0f, 0.0f );
		glTexCoord2f( 0.0f, 1.0f );	glVertex3f( 1.0f, -1.0f, 0.0f );
		glTexCoord2f( 1.0f, 1.0f );	glVertex3f( -1.0f, -1.0f, 0.0f );
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);


}

void _scene::after_render()
{
	//Go back to 3D Projection
	glPopAttrib();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

	glColor4f(1,1,1,1);
	glDisable(GL_DEPTH_TEST);		
}

void _scene::start_shading()
{
	memset(m_fbo.dual_buffer[0],0,sizeof(_fragment)*m_fbo.texture.pixels);
	for ( int y=0; y<m_pbo.get_height(); y+=4 )
	{
		int y1 = y;
		int y2 = y+4;
		y2 = MIN(y2, m_pbo.get_height()-1);
		_shader_engine::singleton().add_job( _fragment_job(y1,y2,&m_fbo) );
	}
	_shader_engine::singleton().start();
}


void _scene::load_scene()
{
	_dgraph::singleton().flush();
	s_camera_pos = vec3f(0,200,0);

	_mesh *model = new _mesh;
	if ( _mesh_mgr::singleton().load_mesh(*model, "data/models/", "sponza.obj")==false )
	{
		LOG("Unable to load model.");
		return;
	}
	_dg_node* root_node = _dgraph::singleton().get_root_node();
	root_node->add_mesh(model);
	root_node->get_matrix().set_translation(vec3f(0,50,0));



}