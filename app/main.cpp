#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/range/irange.hpp>

#include "commandline.h"
#include "Parser.h"
#include "Symbol.h"

namespace {
    void print_credentials() {
        std::cout << "KappaLang parser, version 0.0.0.0.0.0.1a (alpha)" << std::endl;
        std::cout << "Use at your own risk" << std::endl;
    }

    void do_nothing(const Type&) {
    }

    void print_test(const Primitive& primitive) {
        std::copy(primitive.value.begin(), primitive.value.end(), std::ostream_iterator<int64_t>(std::cout, " "));
        std::cout << std::endl;
    }

    void print_primitive(const Primitive& primitive) {
        std::cout << "int " << "(" << primitive.size << ")[" << primitive.value.size() << "] " << primitive.identifier << " = {";
        if (primitive.value.size() > 1)
            std::copy(primitive.value.begin(), primitive.value.end()-1, std::ostream_iterator<int64_t>(std::cout, ","));
        std::cout << primitive.value.back();
        std::cout << "}" << std::endl;
    }

    void print_type(const Type& type) {
        std::cout << type.identifier << ":\n";
    }

    void print_results(std::vector<Type> results, bool test) {
        std::cout << std::endl;
        if (not test) {
            print_credentials();
            for_each(results, 0, print_type, print_primitive);
        }
        else
            for_each(results, 0, do_nothing, print_test);
    }

    void run_program(const auto& opts){
        Parser parser(std::get<0>(opts), std::get<1>(opts));
        print_results(parser.parse(), std::get<2>(opts));
    }
}

int main(int argc, char **argv) {
    //try {
        //const auto [datafile, binfile] RIP
        const auto opts = parse_command_line(argc, argv);
        //std::apply(run_program, files); also rip
        run_program(opts);
    //} catch (const std::exception& e) {
    //    std::cout << e.what() << std::endl;
    //    return -1;
    //}
    return 0;
}
