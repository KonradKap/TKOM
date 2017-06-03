#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>

#include <boost/program_options.hpp>
#include <boost/range/irange.hpp>

#include "commandline/commandline.h"
#include "parser/Parser.h"
#include "utility/add_containers.h"
#include "lexer/Scanner.h"
#include "filler/BinaryReader.h"
#include "filler/Filler.h"
#include "utility/vt100.h"
#include "utility/CompilationError.h"

using namespace std::literals::string_literals;

namespace {
    int clamp_to_zero(int value) {
        return value < 0
            ? 0
            : value;
    }

    void print_credentials() {
        std::cout << "KappaLang parser, version 0.0.0.0.0.1.1a (alpha)" << std::endl;
        std::cout << "Use at your own risk" << std::endl << std::endl;
    }

    void pretty_print_value(const DataArray& data, int level) {
        std::cout << "  "s*level << data.identifier << "[" << data.index << "]" << " = " << data.value << std::endl;
    }

    void pretty_print_type(const DataArray& data, int level) {
        if (data.has_value)
            pretty_print_value(data, level);
        else {
            std::cout << "  "s*level << data.type_name
                << " : " << data.identifier << "[" << data.index << "] {" << std::endl;
            for(const auto& child : data.children)
                pretty_print_type(child, level + 1);
            std::cout << "  "s*level << "}" << std::endl;
        }
    }

    void pretty_print(const DataArray& data) {
        for (const auto& child : data.children)
            pretty_print_type(child, 0);
    }

    void print_test(const DataArray& data) {
        if (data.has_value)
            std::cout << data.value << std::endl;
        else {
            for(const auto& child : data.children)
                print_test(child);
        }
    }

    void print_results(const DataArray& data, bool test) {
        std::cout << std::endl;
        if (not test) {
            print_credentials();
            pretty_print(data);
        } else
            print_test(data);
    }

    std::string getPrettyLine(const CompilationError& error, Scanner& scanner) {
        const auto caret_position = clamp_to_zero(
                error.column_number - 1 - error.length);
        const auto underline_length = clamp_to_zero(error.length - 1);
        return scanner.getLine(error.line_number) + "\n"
            + " "s*caret_position + "^" + "~"s*underline_length;
    }

    void signal_pretty_error(const CompilationError& error, Scanner& scanner) {
        std::cout << vt100::color::Red << "Error on line "
            << error.line_number << ":" << vt100::Reset << std::endl;
        std::cout << vt100::Bold << error.what() << vt100::Reset << std::endl;
        std::cout << getPrettyLine(error, scanner) << std::endl;
    }

    void signal_test_error(const CompilationError& error) {
        std::cout << error.what() << "\n" << error.line_number << " " << error.column_number << std::endl;
    }

    void signal_error(const CompilationError& error, Scanner& scanner, bool test) {
        if (test)
            signal_test_error(error);
        else
            signal_pretty_error(error, scanner);
    }

    DataArray fill_data(std::shared_ptr<Type> root, BinaryReader& reader) {
        DataArray data_root;
        data_root.has_value = false;
        data_root.parent = nullptr;
        root->apply(reader, data_root);
        return data_root;
    }

    void run_program(const std::string& datafile, const std::string binfile, const bool test){
        Scanner scanner{std::make_unique<std::ifstream>(datafile)};
        Parser parser{scanner};
        BinaryReader reader{
            std::make_unique<std::ifstream>(binfile, std::ios_base::binary | std::ios_base::in)};
        try {
            const auto ast = parser.parse();
            const auto data = fill_data(ast, reader);
            print_results(data, test);
        } catch (const CompilationError& error) {
            signal_error(error, scanner, test);
        }
    }
}

int main(int argc, char **argv) {
    try {
        const auto opts = parse_command_line(argc, argv);
        if (not opts.valid())
            return 0;
        run_program(opts.datafile, opts.binfile, opts.test);
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}
