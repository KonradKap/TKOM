#pragma once

#include <exception>
#include <string>
#include <iostream>

#include "parser/ReferenceList.h"

class CompilationError : public std::runtime_error {
    public:
        CompilationError(
                const char *what,
                int line_number = -1,
                int column_number = -1,
                int length = -1) :
            std::runtime_error(what),
            line_number(line_number),
            column_number(column_number),
            length(length) {
        }
        CompilationError(
                const std::string& what,
                int line_number = -1,
                int column_number = -1,
                int length = -1) :
            CompilationError(what.c_str(), line_number, column_number, length) {
        }

        CompilationError(
                const std::string& what,
                const ReferenceList& list) :
            CompilationError(what.c_str(), list) {
        }

        CompilationError(
                const char *what,
                const ReferenceList& list) :
            CompilationError(what, list.line_number, list.column_number, list.length) {

        }

    int line_number;
    int column_number;
    int length;
};
