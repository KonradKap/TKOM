#pragma once

#include <string>
#include <exception>

#include "Scanner.h"

bool isValidIdentifierStart(int character);
bool isValidIdentifierContinuation(int character);
bool isValidIdentifier(const std::string& input);

bool isNumericConstant(const std::string& input);
void signal_error(const Scanner& input, std::string what);

struct typename_equality {
    public:
        typename_equality(const std::string& type_name) :
            type_name(type_name) {
            }

        bool operator()(const auto& t) {
            return t.type_name == type_name;
        }

    private:
        std::string type_name;
};

struct identifier_equality {
    public:
        identifier_equality(const std::string& identifier) :
            identifier(identifier) {
            }

        bool operator()(const auto& t) {
            return t.identifier == identifier;
        }

    private:
        std::string identifier;
};

template<class Exception = std::runtime_error>
    void signal_error(const Scanner& input, std::string what) {
        throw Exception("Error at line "
                + std::to_string(input.getLineNumber())
                + ": "
                + what
                + ", got: '"
                + input.getLastRead()
                + "'");
    }
