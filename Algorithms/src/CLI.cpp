#include <iostream>
#include <fstream>
#include <regex>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/optional/optional_io.hpp>

#include "CLI.hpp"

bool	CLI::isFlagSet(const std::string &flag) const {
	if (this->vm.count(flag))
		return (true);
	return (false);
}

bool	CLI::getFlag(const std::string &flag, int &result) const {
	if (this->vm.count(flag)) {
		try {
			result = this->vm[flag].as<int>();
		}
		catch (std::exception &e) {
			std::cerr << "Error: " << e.what() << std::endl;
			return (false);
		}
		return (true);
	}
	return (false);
}

bool	CLI::getFlag(const std::string &flag, std::string &result) const {
	if (this->vm.count(flag)) {
		try {
			result = this->vm[flag].as<std::string>();
		}
		catch (std::exception &e) {
			std::cerr << "Error: " << e.what() << std::endl;
			return (false);
		}
		return (true);
	}
	return (false);
}

bool	CLI::processArguments(int argc, const char **argv) {
	namespace po = boost::program_options;
	try {
		po::store(po::parse_command_line(argc, argv, this->desc), this->vm);
		po::notify(this->vm);

		this->getFlag("file", this->fileName);

		if (this->vm.count("help")) {
			std::cout << desc << std::endl;
		}
	}
	catch (po::error &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << desc << std::endl;
		return (false);
	}
	return (true);
}

CLI::CLI(int argc, const char **argv) : desc("Options") {
    // TODO: add description
	namespace po = boost::program_options;
	this->desc.add_options()
			("help,h", "Print help")
			("file,f", po::value<std::string>(), "Filename of loaded map")
            ("num,n", po::value<std::string>(), "Number of generations to live (1500 by default)")
            ("verbose,v", po::value<std::string>(), "Verbosity level:\n"
                                                    "\t0 - without prints (default)\n"
                                                    "\t1 - print map on each generation\n"
                                                    "\t2 - print only last generation");

	if (!processArguments(argc, argv))
		throw CLI_invalidArguments();

    if (isFlagSet("file"))
        parseFile();
}

CLI::~CLI() {}

void	CLI::parseFile() {
    namespace qi     = boost::spirit::qi;
    namespace phx    = boost::phoenix;

    std::string     fileName, line;
    std::ifstream   file;
    std::regex      comment("#.*");
    std::regex      dimensions("height = [0-9]+, width = [0-9]+");
    std::regex      cell("(\\.|\\*)*");

    this->getFlag("file", fileName);
    file = std::ifstream(fileName);

    if (!file.good())
        throw CLI_InvalidFile();

    // skip empty lines
    while (std::getline(file, line)) {
        if (std::regex_match(line, comment) || line.empty())
            continue;
        else
            break;
    }

    // parse dimesions line
    auto begin = line.begin();
    auto end = line.end();
    bool ok = qi::phrase_parse
                    (
                        begin,
                        end,
                            "height = " >>
                            qi::int_[phx::ref(height) = qi::_1] >>
                            ", width = " >>
                            qi::int_[phx::ref(width) = qi::_1],
                        qi::space
                    );
    if (!ok)
        throw CLI_InvalidFile();

    this->map.resize(height);
    for (auto &line : this->map)
        line.resize(width);

    // parse cells
    int y = 0;
    while (std::getline(file, line)) {
        if (std::regex_match(line, cell)) {
            for (unsigned x = 0; x < line.length(); x++) {
                line[x] == '.' ? map[y][x] = 0 : map[y][x] = 1;
            }
        } else {
            throw CLI_InvalidMap();
        }
        y++;
    }
}

void	CLI::startLogic() const {

}

unsigned int CLI::getHeight() const {
    return height;
}

unsigned int CLI::getWidth() const {
    return width;
}

const Map &CLI::getMap() const {
    return map;
}
