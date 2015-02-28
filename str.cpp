#include "str.h"

void str_format( char *a_s )
{
	size_t len =strlen(a_s);
	for ( size_t i=0; i<len; i++ )
	{
		if ( a_s[i]>='a'&&a_s[i]<='z' )
		{
			a_s[i] = a_s[i] - 'a' + 'A';
		}
	}
	//remove head space
	size_t spc = 0;
	while ( a_s[spc]==' '||a_s[spc]=='\t' )
		spc++;
	if ( spc!=0 )
	{
		for ( size_t i=0; i<len-spc+1; i++ )
		{
			a_s[i] = a_s[i + spc];
		}
	}
}

str file_base_name(const str path)
{

	size_t len = path.length();
	size_t dot_pos = -1;
	size_t slash_pos = -1;
	for ( size_t i = len -1; i>=0; i-- )
	{
		if ( dot_pos == -1 && slash_pos == -1 && path[i]=='.' )
			dot_pos = i;
		else if( slash_pos == -1 && ( path[i]=='\\'||path[i]=='/') )
		{
			slash_pos = i;
			break;
		}
	}
	if (dot_pos == -1 )
		return path.substr(slash_pos+1, len - slash_pos -1);
	return path.substr( slash_pos+1, dot_pos - slash_pos -1 );

}