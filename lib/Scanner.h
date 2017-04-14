#pragma once

#include <istream>
#include <memory>

#include "Token.h"

class Scanner {
    public:
        struct saved_pos {
            int line_number;
            int column_number;
            std::streampos streampos;
        };

        explicit Scanner(std::unique_ptr<std::istream> input);

        Token getNextToken();
        Token peekNextToken();
        void readUntill(Token stop);

        bool endOfFile();
        int getLineNumber() const;
        int getColumnNumber() const;

        saved_pos getPosition();
        void setPosition(saved_pos new_position);

        std::string getLastRead() const;
        std::vector<std::string> readDeepIdentifier();

    private:
        bool isLongestMatch() const;
        void ignoreWhitespacesAndComments();
        void nextLine();
        void ignoreComments();

        std::unique_ptr<std::istream> input;

        int line_number;
        int column_number;

        std::string last_read;
};
