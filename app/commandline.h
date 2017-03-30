#pragma once

#include <boost/program_options.hpp>

std::tuple<std::string, std::string, bool> parse_command_line(int argc, char **argv);
