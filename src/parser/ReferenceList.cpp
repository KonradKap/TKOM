#include "ReferenceList.h"

#include "utility/print_container.h"

bool ReferenceList::operator==(const ReferenceList& rhs) const {
    return this->elements == rhs.elements;
}

bool ReferenceList::operator!=(const ReferenceList& rhs) const {
    return not(*this == rhs);
}

std::ostream& operator<<(std::ostream& os, const ReferenceList& list) {
    print(list.elements, os);
    return os;
}

bool ReferenceList::empty() const {
    return elements.empty();
}

ReferenceList& ReferenceList::addElement(const Element& element) {
    elements.push_back(element);
    return *this;
}

ReferenceList ReferenceList::pop() const {
    std::vector<Element> new_elements(elements.size() - 1);
    std::copy(elements.begin() + 1, elements.end(), new_elements.begin());
    return ReferenceList{new_elements, line_number, column_number, length};
}

bool operator==(const ReferenceList::Element& lhs, const ReferenceList::Element& rhs) {
    return lhs.list == rhs.list
        and lhs.value == rhs.value;
}

bool operator!=(const ReferenceList::Element& lhs, const ReferenceList::Element& rhs) {
    return not(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const ReferenceList::Element& element) {
    return os << element.list << ":" << element.value;
}
