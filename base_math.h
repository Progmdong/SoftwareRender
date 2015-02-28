#ifndef _BASE_MATH_H
#define _BASE_MATH_H

#include <math.h>

#define PI 3.14159265f

#define LERP(a, b, perc)		( a*(1-perc) + b*perc )
#define  MIN(a,b)				(  (a)<(b)?(a):(b) )
#define  MAX(a,b)			(  (a)>(b)?(a):(b) )
#define CLAMP(a_V,a,b)	MAX( MIN(a_V,(b)), (a) )

#define RED_BIT 0
#define GREEN_BIT 8
#define BLUE_BIT 16
#define ALPHA_BIT 24
#define ALPHA 0xff000000

#define COLOR(r,g,b) ( (r)<<RED_BIT | (g)<<GREEN_BIT | (b)<<BLUE_BIT | ALPHA )
#define COLOR_CLAMP(r,g,b) ((CLAMP(r,0,255)<<RED_BIT) | (CLAMP(g,0,255)<<GREEN_BIT) | (CLAMP(b,0,255)<<BLUE_BIT) | ALPHA )

#define RED_MASK 0x000000ff
#define GREEN_MASK 0x0000ff00
#define BLUE_MASK  0x00ff0000

#define COMPONENT_RED(col) (col&RED_MASK)
#define COMPONENT_GREEN(col) ((col&GREEN_MASK)>>GREEN_BIT)
#define COMPONENT_BLUE(col) ((col&BLUE_MASK)>>BLUE_BIT)


template<class T>
struct vec2
{

	typedef vec2<T> self;

	vec2():x(0.),y(0.){}
	vec2( T a_x, T a_y ):x(a_x),y(a_y){}
	
	void normalize();
	T length() const;
	T dot( const self a_v );

	self min_vec(self a_v);
	self max_vec( const self a_v );
	T& operator[]( size_t a_i ) ;
	self operator - ();
	self operator + ( const self a_v );
	self operator - ( const self a_v );
	self operator * ( const self a_v );
	self operator * ( T a_f  );
	bool operator == ( const self a_v );

	union
	{
		struct{ T x,y; };
		struct{ T u,v; };
		struct{ T cell[2]; };
	};

};



template<class T>
struct vec3
{

	vec3():x(0.),y(0.),z(0.){}
	vec3( T a_x, T a_y, T a_z ):x(a_x),y(a_y),z(a_z){}

	typedef vec3<T> self;

	void normalize();
	T length() const;
	T dot( const self a_v );

	self min_vec(const self a_v);
	self max_vec(const self a_v);
	T& operator[]( size_t a_i ) ;
	self operator - ();
	self operator + ( const self a_v );
	self operator - ( const self a_v );
	self operator * ( const self a_v );
	self operator * ( T a_f  );
	bool operator == ( const self a_v );

	union
	{
		struct{ T x,y,z; };
		struct{ T r,g,b; };
		struct{ T cell[3]; };
	};

};

template<class T>
vec3<T> cross(vec3<T> a, vec3<T> b)
{
	return vec3<T>( a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x );
}


template<class T>
struct mat4x4
{
	typedef mat4x4<T> self;
	mat4x4(){identity();}
	void identity();
	void invert();
	void rotate_x( T  a_rx);
	void rotate_y( T a_ry );
	void rotate_z( T a_rz );
	void scale( T a_s );
	void set_translation(vec3<T> a_v);
	void translate( vec3<T> a_v );
	void concatenate( self &a_m );
	void lookat( vec3<T> a_eye, vec3<T> a_at, vec3<T> a_up );
	void projection( T a_fov, T a_aspect, T a_znear, T a_zfar );
	vec3<T> transform( vec3<T> a_v);
	vec3<T> transform_rotation( vec3<T> a_v );
	self operator * (self &a_m);

	T cell[ 16 ];
};

#include "base_math.h"

template<class T>
T& vec2<T>::operator[]( size_t a_i ) 
{ 
	assert(a_i<2); 
	return cell[a_i]; 
}

template<class T>
T vec2<T>::length() const
{
	return sqrt(x*x+y*y);
}

template<class T>
void vec2<T>::normalize()
{
	T l = length();
	x /= l;
	y /= l;
}

template<class T>
T vec2<T>::dot( const self a_v )
{
	return x*a_V.x + y*a_v.y;
}

template<class T>
vec2<T> vec2<T>::min_vec( const self a_v )
{
	return vec2<T>( MIN(x,a_v.x), MIN(y, a_v.y) );
}

template<class T>
vec2<T> vec2<T>::max_vec( const self a_v )
{
	return vec2<T>( MAX(x,a_v.x), MAX(y, a_v.y) );
}

template<class T>
vec2<T> vec2<T>::operator-()
{
	return vec2<T>( -a_v.x, -a_v.y );
}

template<class T>
vec2<T> vec2<T>::operator+(const self a_v)
{
	return vec2<T>( x+a_v.x, y+a_v.y );
}

template<class T>
vec2<T> vec2<T>::operator-(const self a_v)
{
	return vec2<T>( x-a_v.x, y-a_v.y );
}

template<class T>
vec2<T> vec2<T>::operator*(const self a_v)
{
	return vec2<T>( x*a_v.x, y*a_v.y );
}

template<class T>
vec2<T> vec2<T>::operator*(T a_f)
{
	return vec2<T>( x*a_f, y*a_f );
}

template<class T>
bool vec2<T>::operator==(const self a_v)
{
	return ( (x==a_v.x) && (y==a_v.y) );
}


template<class T>
T& vec3<T>::operator[]( size_t a_i ) 
{ 
	assert(a_i<3); 
	return cell[a_i]; 
}

template<class T>
T vec3<T>::length() const
{
	return sqrt(x*x+y*y+z*z);
}

template<class T>
void vec3<T>::normalize()
{
	T l = length();
	x /= l;
	y /= l;
	z /= l;
}

template<class T>
T vec3<T>::dot( const self a_v )
{
	return x*a_V.x + y*a_v.y + z*a_v.z;
}

template<class T>
vec3<T> vec3<T>::min_vec( const self a_v )
{
	return vec3<T>( MIN(x,a_v.x), MIN(y, a_v.y), MIN(z, a_v.z) );
}

template<class T>
vec3<T> vec3<T>::max_vec( const self a_v )
{
	return vec3<T>( MAX(x,a_v.x), MAX(y, a_v.y), MAX(z, a_v.z) );
}

template<class T>
vec3<T> vec3<T>::operator-()
{
	return vec3<T>( -x, -y, -z );
}

template<class T>
vec3<T> vec3<T>::operator+(const self a_v)
{
	return vec3<T>( x+a_v.x, y+a_v.y, z+a_v.z );
}

template<class T>
vec3<T> vec3<T>::operator-(const self a_v)
{
	return vec3<T>( x-a_v.x, y-a_v.y, z-a_v.z );
}

template<class T>
vec3<T> vec3<T>::operator*(const self a_v)
{
	return vec3<T>( x*a_v.x, y*a_v.y, z*a_v.z );
}

template<class T>
vec3<T> vec3<T>::operator*(T a_f)
{
	return vec3<T>( x*a_f, y*a_f, z*a_f );
}

template<class T>
bool vec3<T>::operator==(const self a_v)
{
	return ( (x==a_v.x) && (y==a_v.y) && (z==a_v.z) );
}

template<class T>
void mat4x4<T>::identity()
{
	cell[0] = cell[5] = cell[10] = cell[15] = 1;
	cell[1] = cell[2] = cell[3] = cell[4] = cell[6]
	= cell[7] = cell[8] = cell[9] = cell[11]
	= cell[12] = cell[13] = cell[14] = 0;
}

template<class T>
void mat4x4<T>::concatenate( self &a_m )
{
	self res;
	int c;
	for ( c = 0; c < 4; c++ ) for ( int r = 0; r < 4; r++ )
		res.cell[r * 4 + c] = cell[r * 4] * a_m.cell[c] +
		cell[r * 4 + 1] * a_m.cell[c + 4] +
		cell[r * 4 + 2] * a_m.cell[c + 8] +
		cell[r * 4 + 3] * a_m.cell[c + 12];
	for ( c = 0; c < 16; c++ ) cell[c] = res.cell[c];
}

template<class T>
void mat4x4<T>::rotate_x(T a_rx)
{
	self m = *this;
	float s = (float)sin(a_rx*PI/180);
	float c = (float)cos(a_rx*PI/180);
	identity();
	cell[5] = c, cell[6] = s, cell[9] = -s, cell[10] = c;
	concatenate(m);
}

template<class T>
void mat4x4<T>::rotate_y(T a_ry)
{
	self m = *this;
	float s = (float)sin(a_ry*PI/180);
	float c = (float)cos(a_ry*PI/180);
	identity();
	cell[0] = c, cell[2] = -s, cell[8] = s, cell[10] = c;
	concatenate(m);
}

template<class T>
void mat4x4<T>::rotate_z(T a_rz)
{
	self m = *this;
	float s = (float)sin(a_rz*PI/180);
	float c = (float)cos(a_rz*PI/180);
	identity();
	cell[0] = c, cell[1] = s, cell[4] = -s, cell[5] = c;
	concatenate(m);
}

template<class T>
void mat4x4<T>::translate(vec3<T> a_v)
{
	cell[3] += a_v.x;
	cell[7] += a_v.y;
	cell[11] += a_v.z;
}

template<class T>
void mat4x4<T>::set_translation(vec3<T> a_v)
{
	cell[3] = a_v.x;
	cell[7] = a_v.y;
	cell[11] = a_v.z;
}

template<class T>
void mat4x4<T>::scale(T a_s)
{
	cell[0] *= a_s;
	cell[5] *= a_s;
	cell[10] *= a_s;
}

template<class T>
vec3<T> mat4x4<T>::transform(vec3<T> a_V)
{
	T x  = cell[0] * a_V.x + cell[1] * a_V.y + cell[2] * a_V.z + cell[3];
	T y  = cell[4] * a_V.x + cell[5] * a_V.y + cell[6] * a_V.z + cell[7];
	T z  = cell[8] * a_V.x + cell[9] * a_V.y + cell[10] * a_V.z + cell[11];
	return vec3<T>( x, y, z );
}

template<class T>
vec3<T> mat4x4<T>::transform_rotation( vec3<T> a_V )
{
	T x  = cell[0] * a_V.x + cell[1] * a_V.y + cell[2] * a_V.z;
	T y  = cell[4] * a_V.x + cell[5] * a_V.y + cell[6] * a_V.z;
	T z  = cell[8] * a_V.x + cell[9] * a_V.y + cell[10] * a_V.z;
	return vec3<T>( x, y, z );
}

template<class T>
mat4x4<T> mat4x4<T>::operator * ( self& m2 )
{
	self res;
	int c;
	for ( c = 0; c < 4; c++ ) for ( int r = 0; r < 4; r++ )
		res.cell[r * 4 + c] = cell[r * 4] * m2.cell[c] +
		cell[r * 4 + 1] * m2.cell[c + 4] +
		cell[r * 4 + 2] * m2.cell[c + 8] +
		cell[r * 4 + 3] * m2.cell[c + 12];

	return res;
}

template<class T>
void mat4x4<T>::lookat( vec3<T> a_eye, vec3<T> a_at, vec3<T> a_up )
{
	identity();
	vec3<T> forward = a_at - a_eye;
	forward.normalize();
	vec3<T> side = cross(forward, a_up);
	cell[0] = side.x;	cell[1] = a_up.x;		cell[2] = -forward.x;	
	cell[4] = side.y;	cell[5] = a_up.y;		cell[6] = -forward.y;	
	cell[8] = side.z;	cell[9] = a_up.z;		cell[10] = -forward.z;	

	cell[3] = -a_eye.x;
	cell[7] = -a_eye.y;
	cell[11] = -a_eye.z;
}

template<class T>
void mat4x4<T>::projection( T a_fov, T a_aspect, T a_znear, T a_zfar )
{
	identity();

	const float h = 1.0f / tan(a_fov * (PI / 360));
	float neg_depth = a_znear - a_zfar;
	//openGL style
	cell[0] = h / a_aspect;
	cell[5] = h;
	cell[10] = (a_zfar + a_znear)/neg_depth;
	cell[11] = 2.0f * (a_znear * a_zfar) / neg_depth;;
	cell[14] = -1;
}

template<class T>
void mat4x4<T>::invert()
{
	mat4x4 t;
	int h, i;
	float tx = -cell[3], ty = -cell[7], tz = -cell[11];
	for ( h = 0; h < 3; h++ ) for ( int a_V = 0; a_V < 3; a_V++ ) t.cell[h + a_V * 4] = cell[a_V + h * 4];
	for ( i = 0; i < 11; i++ ) cell[i] = t.cell[i];
	cell[3] = tx * cell[0] + ty * cell[1] + tz * cell[2];
	cell[7] = tx * cell[4] + ty * cell[5] + tz * cell[6];
	cell[11] = tx * cell[8] + ty * cell[9] + tz * cell[10];
}

typedef vec2<float> vec2f;
typedef vec3<float> vec3f;
typedef mat4x4<float> mat4x4f;
#endif