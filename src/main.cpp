#include <iostream>
#include <fstream>
#include <stdexcept>
#include <boost/asio/ssl.hpp>
#include <boost/program_options.hpp>
#include "tamandua.hpp"
#include "version.hpp"

int main(int argc, char **argv)
{
	std::vector<std::string> modules;
	std::string config_file, chain, pk, dh, logfile_name;
	char cmd_fc;
	unsigned int port;
	std::ostream *logstream = &(std::cerr);

	boost::program_options::options_description opts("TMDSvr options");
	opts.add_options()
		("help", "help information")
		("version,v", "version information")
		("module,M", boost::program_options::value<std::vector<std::string>>(&modules), "additional modules")
		("port", boost::program_options::value<unsigned int>(&port)->default_value(5000)->required(), "port")
		("ssl-chain", boost::program_options::value<std::string>(&chain)->required(), "ssl certificate chain file")
		("ssl-pk", boost::program_options::value<std::string>(&pk)->required(), "ssl private key")
		("ssl-dh", boost::program_options::value<std::string>(&dh)->required(), "ssl dhparam file")
		("logfile,l", boost::program_options::value<std::string>(&logfile_name), "log file (std::cerr if not set)")
		(",c", boost::program_options::value<char>(&cmd_fc)->default_value('/'), "first character of command")
		("config", boost::program_options::value<std::string>(&config_file), "config file")
	;

	boost::program_options::variables_map vm;
	try {
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opts), vm);

		if (vm.count("help"))
		{
			std::cout << opts << "\n";
			return 1;
		}

		if (vm.count("version"))
		{
			std::cout << "TMDSvr " << get_version_string() << " uses Tamandua " << tamandua::get_version_str() << "\n";
			return 1;
		}

		boost::program_options::notify(vm);
	} catch (std::exception &exc)
	{
		std::cerr << "Error: " << exc.what() << "\n";
		return 1;
	} catch (...)
	{
		std::cerr << "Unknown error!\n";
		return 1;
	}

	boost::asio::io_service io_service;
	boost::asio::ssl::context ssl_ctx(boost::asio::ssl::context::sslv23);
	ssl_ctx.set_options(
		boost::asio::ssl::context::default_workarounds |
		boost::asio::ssl::context::no_sslv2 |
		boost::asio::ssl::context::single_dh_use);
	ssl_ctx.use_certificate_chain_file(chain);
	ssl_ctx.use_private_key_file(pk, boost::asio::ssl::context::pem);
	ssl_ctx.use_tmp_dh_file(dh);

	std::fstream logfile;
	if (vm.count("logfile"))
	{
		logfile.open(logfile_name, std::ios::app | std::ios::out);
		if (!logfile.is_open())
		{
			throw std::runtime_error("could not open given logfile!");
		}
		logstream = &logfile;
	}

	tamandua::logger logger(*logstream);
	tamandua::command_interpreter command_interpreter(cmd_fc);
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
	tamandua::server_config conf;
	if (vm.count("config"))
	{
		std::fstream cfg_fstream(config_file, std::ios::in);
		if (!cfg_fstream.is_open())
		{
			throw std::runtime_error("could not open given config file!");
		}
		boost::program_options::options_description cfg_opts;
		cfg_opts.add_options()
			("server_name", boost::program_options::value<std::string>(&conf.server_name), "server name")
			("max_participants_number", boost::program_options::value<size_t>(&conf.max_participants_number), "max number of connections")
			("main_room_name", boost::program_options::value<std::string>(&conf.main_room_name), "name of room with id = 0")
			("root_password", boost::program_options::value<std::string>(&conf.root_password), "root password")
			("username_regex", boost::program_options::value<std::string>(&conf.participant_name_format), "regular expression for username testing")
		;

		boost::program_options::variables_map conf_vm;
		boost::program_options::store(boost::program_options::parse_config_file(cfg_fstream, cfg_opts), conf_vm);
		boost::program_options::notify(conf_vm);
	}
	if (conf.server_name.empty())
	{
		std::cout << "Server name: ";
		std::getline(std::cin, conf.server_name);
	}
	if (conf.max_participants_number < 1)
	{
		std::cout << "Maximum participants number: ";
		std::cin >> conf.max_participants_number;
	}
	if (conf.main_room_name.empty())
	{
		std::cout << "Main room name: ";
		std::getline(std::cin, conf.main_room_name);
	}
	if (conf.root_password.empty())
	{
		std::cout << "Root password: ";
		std::getline(std::cin, conf.root_password);
	}
	
	tamandua::server server(io_service, endpoint, logger, command_interpreter, ssl_ctx);
	// register modules
	tamandua::base_user_module base(server, command_interpreter);
	server.register_module(base);

	server.start_server(conf);
	io_service.run();

	return 0;
}
