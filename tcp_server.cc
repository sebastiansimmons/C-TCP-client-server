/*
tcp_server.cc 
*/

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <string>


int main(int ac, char* av[])
{
    try {

        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
        ;

        // Declare a group of options that will be 
		// allowed both on command line and in
		// config file
		po::options_description config("Configuration");
		config.add_options()
		    ("maxmem,m", po::value<int>(&opt)->default_value(1024), 
		          "maximum memory allowed in cache")
		    ("server,s", po::value<std::string>(&opt)->default_value("127.0.0.1"), 
		          "the ip to run the server on")
		    ("port,p", po::value<int>(&opt)->default_value(6300), 
		          "the port to run the server on")
		    ("threads,t", po::value<int>(&opt)->default_value(6300), 
		          "the number of threads")
		    ;

        po::variables_map vm;        
        po::store(po::parse_command_line(ac, av, desc, config), vm);
        po::notify(vm);    

        if (vm.count("help")) {
            std::cout << desc << std::endl << config << std::endl;
            return 0;
        }
        // TODO: add output for other options
    }
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
    }

    return 0;
}
