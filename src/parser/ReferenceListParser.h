#pragma once

#include "Parser.h"

class ReferenceListParser : public Parser {
    public:
        ReferenceListParser(Scanner& s);

        ReferenceList parse();
};
