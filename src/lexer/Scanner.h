#pragma once

#include <istream>
#include <memory>
#include <vector>

#include "Token.h"

class Scanner {
    public:
        explicit Scanner(std::unique_ptr<std::istream> input);

        Token getNextToken();
        Token peekNextToken();
        void readUntill(Token stop);

        bool endOfFile();
        int getLineNumber() const;
        int getColumnNumber() const;

        std::string getLine(int number);

        std::string getLastRead() const;
        std::vector<std::string> readDeepIdentifier();

    private:
        struct saved_position {
            int line_number;
            int column_number;
            std::streampos streampos;
            std::string last_read;
        };

        class ScopedSave {
            public:
                ScopedSave(Scanner& state);
                ScopedSave(Scanner& state, const saved_position& position);
                ~ScopedSave();
            private:
                Scanner& state;
                const saved_position save;
        };

        saved_position getPosition();
        void setPosition(saved_position new_position);

        bool isLongestMatch() const;
        void ignoreWhitespacesAndComments();
        void nextLine();
        void ignoreComments();
        void goToBeginning();

        std::unique_ptr<std::istream> input;

        int line_number;
        int column_number;

        std::string last_read;
};
