#pragma once

#include <vector>
#include <string>
#include <ostream>

#include "lexer/Scanner.h"

class ReferenceList {
    public:
        struct Element;
        struct LengthMeasure;

        bool operator==(const ReferenceList& rhs) const;
        bool operator!=(const ReferenceList& rhs) const;
        friend std::ostream& operator<<(std::ostream& os, const ReferenceList& list);

        bool empty() const;
        ReferenceList& addElement(const Element& element);
        ReferenceList pop() const;

        std::vector<Element> elements;
        int line_number = -1;
        int column_number = -1;
        int length = 0;
};

struct ReferenceList::Element {
    ReferenceList list;
    std::string value;
};

struct ReferenceList::LengthMeasure {
    public:
        LengthMeasure(const Scanner& scanner, ReferenceList& list) :
            begin(scanner.getColumnNumber()),
            scanner(scanner),
            list(list) {
        }
        ~LengthMeasure() {
            list.length = scanner.getColumnNumber() - begin;
        }
    private:
        const int begin;
        const Scanner& scanner;
        ReferenceList& list;
};

bool operator==(const ReferenceList::Element& lhs, const ReferenceList::Element& rhs);
bool operator!=(const ReferenceList::Element& lhs, const ReferenceList::Element& rhs);
std::ostream& operator<<(std::ostream& os, const ReferenceList::Element& element);
