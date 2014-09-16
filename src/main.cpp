#include <iostream>
#include <boost/asio/ssl.hpp>
#include <boost/program_options.hpp>
#include "tamandua.hpp"

int main(int argc, char **argv)
{
	boost::program_options::options_description opts("TMDSvr options:");
	opts.add_options()
		("help", "help information")
		("version,v", "version information")
		("module,M", boost::program_options::value<std::vector<std::string>>(), "additional modules")
		("config,C", "configuration file")
	;

	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opts), vm);
	boost::program_options::notify(vm);

	if (vm.count("help"))
	{
		std::cout << opts << "\n";
		return 1;
	}

	if (vm.count("version"))
	{
		std::cout << "TMDSvr uses Tamandua " << tamandua::get_version_str() << "\n";
		return 1;
	}



	return 0;
}
