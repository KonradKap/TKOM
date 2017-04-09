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

        void parse();

        void for_each(std::function<void (Type&)> on_type, std::function<void (Primitive&)> on_primitive);

    private:
        void readTokens(std::vector<Token>::const_iterator& begin, int count);
        void readTokens(std::vector<Token>::const_iterator& begin, int count, Scanner& input) const;

        void signal_error(std::string what) const;
        void parse_data(Type& parent);
        bool parse_structid(Type& parent);
        bool parse_unionid(Type& parent);
        bool parse_declaration(Type& parent);
        void parse_reuse_of_id(Type& parent);
        void parse_int_declaration(Type& parent);
        void parse_caseblock(int decider, Type& parent);
        bool parse_case(int decider, Type& parent);
        void discard_datablock();

        std::pair<bool, int64_t> checkForIntegerValue(const Type& parent, Scanner& input, const std::vector<Token>& brackets) const;
        int64_t expectIntegerValue(const Type& parent, Scanner& input, const std::vector<Token>& brackets) const;
        std::string checkForIdentifier(const Type& context);
        int64_t readValue(const Type& parent, Scanner& input) const;
        void expectDataBlock(Type& parent);

        Scanner scanner;
        BinaryReader binaryReader;

        Type root;
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
