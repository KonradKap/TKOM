#pragma once

#include <vector>
#include <memory>

#include "Type.h"
#include "Integer.h"
#include "Union.h"
#include "ReferenceList.h"

#include "lexer/Token.h"

class Scanner;
typedef std::vector<std::string> argument_list_t;

class Parser {
    public:
        Parser(Scanner& scanner);

        std::shared_ptr<Type> parse();

        bool expectToken(Token expected);
        bool checkToken(Token expected);
        bool checkToken(const std::vector<Token>& tokens);
        void checkListEnd();

        void parseDataBlock(std::shared_ptr<Type> parent);
        void parseData(std::shared_ptr<Type> parent);
        void parseStructDeclaration(std::shared_ptr<Type> parent);
        void parseUnionDeclaration(std::shared_ptr<Type> parent);
        void parseUnionBody(std::shared_ptr<Union> new_union);
        void checkCases(std::shared_ptr<Union> new_union);
        void checkDefault(std::shared_ptr<Union> new_union);
        std::pair<argument_list_t, std::string> parseDeclarationBegin(std::shared_ptr<Type> parent, Token begin);

        void parseIdentifierUsage(std::shared_ptr<Type> parent);
        void parseIntegerUsage(std::shared_ptr<Type> parent);
        void parseUsage(std::shared_ptr<Type> parent, std::shared_ptr<Type> source);

        argument_list_t parseArgumentsList();
        ReferenceList parseReferenceList(std::pair<Token, Token> brackets);

        std::vector<ReferenceList> checkForArguments(const std::vector<ReferenceList>& default_value = {});
        ReferenceList checkForArray(const ReferenceList& default_value = {});
        std::string checkForIdentifier(const std::string& default_value = "");
        argument_list_t checkForArgumentsList(argument_list_t default_value = {});

        ReferenceList& parseReferenceList(ReferenceList& source);
        void parseSubList(ReferenceList& parent, Token begin, Token end);
        void checkForArguments(std::vector<ReferenceList>& ret, int);

        Scanner& scanner;

        std::shared_ptr<Type> root;
        std::shared_ptr<Integer> Int;
};
