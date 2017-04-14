#pragma once

#include <string>
#include <fstream>
#include <exception>
#include <vector>
#include <cstdint>
#include <tuple>

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

        void signal_error(std::string what) const;
        void parse_data(Type& parent);
        bool parse_structid(Type& parent, bool redeclaration);
        bool parse_unionid(Type& parent, bool redeclaration);
        bool parse_declaration(Type& parent);
        void parse_reuse_of_id(Type& parent);
        void parse_int_declaration(Type& parent);
        void parse_caseblock(int decider, Type& parent);
        bool parse_case(int decider, Type& parent);
        void discard_datablock();

        std::pair<bool, int64_t> checkForIntegerValue(const Type& context, const std::vector<Token>& brackets);
        int64_t expectIntegerValue(const Type& parent, const std::vector<Token>& brackets);
        std::string checkForIdentifier(const Type& context);
        int64_t readValue(const Type& parent);
        void expectDataBlock(Type& parent);
        void copy_type(const Type& original, Type& parent);
        void handle_redeclaration(bool redeclaration, Type& parent, const Type& original);
        void parse_array_and_identifier(Type& parent, const Type& original);
        void discard_array_and_identifier();
        void copy_type_n(int count, Type& parent, const Type& original);

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
