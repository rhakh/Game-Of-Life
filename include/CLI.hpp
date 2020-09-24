#ifndef COMMAND_LINE_INTERFACE_HPP
#define COMMAND_LINE_INTERFACE_HPP

#include <boost/program_options.hpp>
#include <string>
#include <vector>
#include <stdint.h>

namespace po = boost::program_options;

class CLI {
    po::options_description desc;
    po::variables_map       vm;

    std::string             fileName;
    std::vector<uint8_t>    map;
    unsigned int            height, width;

    bool    processArguments(int argc, const char **argv);
    void	parseFile();
public:
    CLI(int argc, const char **argv);
    ~CLI();

    void    startLogic() const;
    bool    isFlagSet(const std::string &flag) const;
    bool    getFlag(const std::string &flag, std::string &result) const;
    bool    getFlag(const std::string &flag, int &result) const;

    unsigned int getHeight() const;
    unsigned int getWidth() const;
    const std::vector<uint8_t> &getMap() const;

    class   CLI_invalidArguments : public std::exception {
    public:
        virtual const char *what() const throw() { return ("Invalid argument"); };
    };

    class   CLI_flagNotSet : public std::exception {
    public:
        virtual const char *what() const throw() { return ("Requested flag was not set"); };
    };

    // Custom exceptions
    class   CLI_InvalidFile : public std::exception {
    public:
        virtual const char *what() const throw() {return ("Invalid file");};
    };

    class   CLI_InvalidMap : public std::exception {
    public:
        virtual const char *what() const throw() {return ("Invalid map");};
    };
};

#endif // COMMAND_LINE_INTERFACE_HPP