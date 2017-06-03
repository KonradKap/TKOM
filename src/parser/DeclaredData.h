#pragma once

#include <string>
#include <memory>

#include "ReferenceList.h"

class Type;

struct DeclaredData {
    std::weak_ptr<Type> type;
    std::string identifier;
    ReferenceList count;
    std::vector<ReferenceList> appliedArguments;

    std::string type_name() const;

    const static ReferenceList defaultCount;
};

bool operator==(const DeclaredData& lhs, const DeclaredData& rhs);
bool operator!=(const DeclaredData& lhs, const DeclaredData& rhs);
std::ostream& operator<<(std::ostream& os, const DeclaredData& obj);
