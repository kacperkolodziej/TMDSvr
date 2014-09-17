#include "version.hpp"
#include "version_config.hpp"

std::string get_version_string()
{
	std::stringstream stream;
	stream << "v" << TMDSvr_VERSION_MAJOR << "." << TMDSvr_VERSION_MINOR << "." << TMDSvr_VERSION_PATCH << "-" << TMDSvr_RELEASE_TYPE;
	return stream.str();
}
