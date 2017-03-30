#pragma once

#include <string>
#include <fstream>
#include <exception>
#include <vector>
#include <cstdint>

#include "Scanner.h"
#include "Data.h"
#include "BinaryReader.h"

class Parser {
    public:
        Parser(std::string datafilename, std::string binfilename);

        const std::vector<Type>& parse();

    private:
        void readTokens(std::vector<Token>::const_iterator& begin, int count);

        auto findTypeByIdentifier(const std::string& identifier) const;
        auto findTypeByIdentifier(const std::string& identifier);
        auto findPrimitiveByIdentifier(const std::string& identifier) const;

        bool wasDeclared(const std::string& identifier) const;
        bool hasValue(const std::string& identifier) const;
        bool isDataStructure(const std::string& identifier) const;

        void signal_error(std::string what) const;
        void parse_data(int parent);
        bool parse_structid(int parent);
        bool parse_unionid(int parent);
        bool parse_declaration(int owner);
        void parse_reuse_of_id(int owner);
        void parse_int_declaration(int owner);
        void parse_caseblock(int value, int parent);
        bool parse_case(int value, int parent);

        std::pair<bool, int64_t> checkForIntegerId(const std::vector<Token>& brackets);
        void checkForIdentifier(auto& data);
        void expectDataBlock(int parent);

        Scanner scanner;
        BinaryReader binaryReader;

        std::vector<Type> declared;
};

class ParserException : public std::runtime_error {
    public:
        explicit ParserException(const char *what) : std::runtime_error(what) {
        }
        explicit ParserException(const std::string& what) : std::runtime_error(what) {
        }
};

void for_each(
        std::vector<Type>& input, size_t begin,
        std::function<void (Type&)> on_type,
        std::function<void (Primitive&)> on_primitive);
