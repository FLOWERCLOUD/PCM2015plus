#include "BASEReader.h"

//== NAMESPACES ===============================================================


namespace OpenMesh {
	namespace IO {
		static inline char tolower(char c) 
		{
			using namespace std;
			return ::tolower(c); 
		}
	}
}



std::string PCM::IO::BaseReader::get_description() const
{
	return "";
}

std::string PCM::IO::BaseReader::get_extensions() const
{
	return "";
}

bool PCM::IO::BaseReader::read(const std::string& _filename, BaseImporter& _bi, Options& _opt)
{
	return true;
}

bool PCM::IO::BaseReader::read(std::istream& _is, BaseImporter& _bi, Options& _opt)
{
	return true;
}

bool PCM::IO::BaseReader::can_u_read(const std::string& _filename) const
{
	return true;
}

bool PCM::IO::BaseReader::check_extension(const std::string& _fname, const std::string& _ext) const
{
	return true;
}
