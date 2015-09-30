#ifndef _FILE_IO_H
#define _FILE_IO_H
#include "sample.h"

namespace FileIO
{
	enum FILE_TYPE{ XYZ,PLY,OBJ,NONE };

	Sample* load_point_cloud_file( std::string filename, FILE_TYPE type, IndexType );

}

#endif