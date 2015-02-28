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
	return vec3<T>( -a_v.x, -a_v.y, -a_v.z );
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
void mat4x4<T>::scale(T a_s)
{
	cell[0] *= a_s;
	cell[5] *= a_s;
	cell[10] *= a_s;
}

template<class T>
vec3<T> mat4x4<T>::transform(vec3<T> a_v)
{
	T x  = cell[0] * v.x + cell[1] * v.y + cell[2] * v.z + cell[3];
	T y  = cell[4] * v.x + cell[5] * v.y + cell[6] * v.z + cell[7];
	T z  = cell[8] * v.x + cell[9] * v.y + cell[10] * v.z + cell[11];
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
	vec3<T> side = corss(forward, a_up);
	cell[0] = side.x;	cell[1] = up.x;		cell[2] = -forward.x;	
	cell[4] = side.y;	cell[5] = up.y;		cell[6] = -forward.y;	
	cell[8] = side.z;	cell[9] = up.z;		cell[10] = -forward.z;	

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
	for ( h = 0; h < 3; h++ ) for ( int v = 0; v < 3; v++ ) t.cell[h + v * 4] = cell[v + h * 4];
	for ( i = 0; i < 11; i++ ) cell[i] = t.cell[i];
	cell[3] = tx * cell[0] + ty * cell[1] + tz * cell[2];
	cell[7] = tx * cell[4] + ty * cell[5] + tz * cell[6];
	cell[11] = tx * cell[8] + ty * cell[9] + tz * cell[10];
}