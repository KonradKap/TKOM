#pragma once

#include <map>
#include <list>
#include <vector>
#include <string>
#include <ostream>
#include <functional>

#include <boost/optional.hpp>

#include "parser/ReferenceList.h"

class DataArray {
    public:
        std::string identifier;
        std::string type_name;
        int index;
        std::map<std::string, int> applied_arguments;
        DataArray* parent;

        bool has_value;
        std::list<DataArray> children;
        int value;

        const DataArray* climbUp(const std::string& where) const;
        int climbDown(const ReferenceList& list) const;
        std::list<DataArray>::const_iterator nearestChild(const std::string& identifier) const;
        int getIndex(const ReferenceList::Element& element) const;
        boost::optional<int> handleEmptySublist(const ReferenceList& list) const;
        boost::optional<int> handleEndCases(const ReferenceList& list) const;
        int rollback(const ReferenceList& list) const;
        std::list<DataArray>::const_iterator findArrayBegin(const ReferenceList& element) const;

        void resolveArguments(
                const std::vector<std::string>& args,
                const std::vector<ReferenceList>& arguments);
        int resolveArgument(const ReferenceList& list) const;

        static std::function<bool (const DataArray&, std::string)> identifier_equality;
};

bool operator==(const DataArray& lhs, const DataArray& rhs);
bool operator!=(const DataArray& lhs, const DataArray& rhs);
std::ostream& operator<<(std::ostream& os, const DataArray& lhs);
