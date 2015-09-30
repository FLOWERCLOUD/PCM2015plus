#include "file_system.h"

std::string FileSystem::extension(const std::string& path)
{
	if (path == "")
		return "";

	size_t len = path.length();
	size_t dot_pos = -1;

	for(size_t i= len-1; i>=0; i--)
	{
		if(path[i]=='.')
		{
			dot_pos = i;
			return path.substr( dot_pos+1, len - dot_pos -1 );
		}
	}
	return "";
}

std::string FileSystem::base_name(const std::string& path)
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

std::string FileSystem::dir_name( const std::string& path )
{
	if (path == "")
		return "";

	size_t len = path.length();
	size_t dot_pos = -1;
	size_t slash_pos = -1;
	for ( size_t i = len -1; i>=0; i-- )
	{
		if( slash_pos == -1 && ( path[i]=='\\'||path[i]=='/') )
		{
			slash_pos = i;
			break;
		}
		else if ( dot_pos == -1 && slash_pos == -1 && path[i]=='.' )
			dot_pos = i;

	}
	if(dot_pos==-1)
		return ".";
	if (slash_pos == -1)
	{
		return path;
	}
	return path.substr(0, slash_pos);
}