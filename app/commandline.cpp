#include "commandline.h"

#include <iostream>
#include <tuple>
#include <string>

namespace bpo = boost::program_options;

namespace {
    bpo::options_description make_description() {
        bpo::options_description description("Possible options");
        description.add_options()
            ("help,h", "Prints this message")
            ("datafile,d", bpo::value<std::string>()->required(), "Path to file with data declaration")
            ("binfile,b", bpo::value<std::string>()->required(), "Path to file with binary data")
            ("test,t", bpo::value<bool>()->default_value(false)->implicit_value(true), "Produce output more suitable for tests")
        ;
        return description;
    }

    bpo::variables_map make_variables(const bpo::options_description& description, int argc, char **argv) {
        bpo::variables_map variables;
        bpo::store(bpo::parse_command_line(argc, argv, description), variables);
        if (variables.count("help")) {
            std::cout << description << std::endl;
            return {};
        }
        bpo::notify(variables);
        return variables;
    }
}

std::tuple<std::string, std::string, bool> parse_command_line(int argc, char **argv) {
    const auto description = make_description();
    const auto variables = make_variables(description, argc, argv);
    if (not variables.empty())
        return std::make_tuple(
                variables["datafile"].as<std::string>(),
                variables["binfile"].as<std::string>(),
                variables["test"].as<bool>()
            );
    return std::make_tuple("", "", false);
}
