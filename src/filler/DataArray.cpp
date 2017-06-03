#include "DataArray.h"

#include <algorithm>

#include "utility/CompilationError.h"
#include "utility/utility.h"

const DataArray* DataArray::climbUp(const std::string& where) const {
    auto* p = parent;
    for (; p and p->identifier != where; p = p->parent) {
        const auto it = p->nearestChild(where);
        if (it != p->children.end())
            return std::addressof(*it);
    }

    return p;
}

void DataArray::resolveArguments(
        const std::vector<std::string>& args_id,
        const std::vector<ReferenceList>& arguments) {

    std::transform(arguments.begin(), arguments.end(), args_id.begin(),
        std::inserter(applied_arguments, applied_arguments.end()),
        [this](const ReferenceList& list, const std::string arg) {
            return std::make_pair(arg, resolveArgument(list));
        });
}

int DataArray::getIndex(const ReferenceList::Element& element) const {
    return element.list.empty()
        ? 0
        : resolveArgument(element.list);
}

boost::optional<int> DataArray::handleEndCases(const ReferenceList& list) const {
    if (list.empty()) {
        if (has_value)
            return value;
        throw CompilationError("Invalid request for value in " + type_name, list);
    }
    if (list.elements.front().list.empty()) {
        return handleEmptySublist(list);
    }
    return {};
}

boost::optional<int> DataArray::handleEmptySublist(const ReferenceList& list) const {
    const auto& element = list.elements.front();
    if (isNumericConstant(element.value))
        return boost::make_optional<int>(std::stoi(element.value));
    const auto it = applied_arguments.find(element.value);
    if (it != applied_arguments.end() and list.elements.size() == 1)
        return boost::make_optional<int>(it->second);
    return {};
}

std::list<DataArray>::const_iterator
DataArray::findArrayBegin(const ReferenceList& list) const {
    const auto& element = list.elements.front();
    const auto it = nearestChild(element.value);
    if (it == children.end())
            throw CompilationError("Couldn't find " + element.value, list);
    return it;
}

std::list<DataArray>::const_iterator DataArray::nearestChild(const std::string& identifier) const {
    return std::find_if(children.begin(), children.end(),
        [identifier](const DataArray& data) { return data.identifier == identifier; });
}

int DataArray::resolveArgument(const ReferenceList& list) const {
    const auto opt_value = handleEndCases(list);
    if (opt_value)
        return *opt_value;
    const auto present_child = nearestChild(list.elements.front().value);
    if (present_child != children.end())
        return climbDown(list);
    else
        return rollback(list);
}

int DataArray::rollback(const ReferenceList& list) const {
    const auto& identifier = list.elements.front().value;
    const auto* upper = climbUp(identifier);
    if (upper == nullptr)
        throw CompilationError("Couldn't find " + identifier, list);
    return upper->parent->climbDown(list);
}

int DataArray::climbDown(const ReferenceList& list) const {
    const auto opt_value = handleEndCases(list);
    if (opt_value)
        return *opt_value;
    const auto& element = list.elements.front();
    const auto it = findArrayBegin(list);
    return std::next(it, getIndex(element))->climbDown(list.pop());
}

bool operator==(const DataArray& lhs, const DataArray& rhs) {
    return lhs.identifier == rhs.identifier
        and lhs.value == rhs.value
        and lhs.children == rhs.children;
}

bool operator!=(const DataArray& lhs, const DataArray& rhs) {
    return not (lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const DataArray& lhs) {
    return os;
}
