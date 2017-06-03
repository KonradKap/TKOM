#pragma once

#include <boost/program_options.hpp>

#include <string>

struct command_line_opts {
    std::string datafile;
    std::string binfile;
    bool test;

    bool valid() const;
};

command_line_opts parse_command_line(int argc, char **argv);
