#include "Struct.h"

#include <functional>
#include <algorithm>
#include <boost/range/irange.hpp>

#include "filler/Filler.h"

using namespace std::placeholders;

Struct::Struct() :
    Type{} {
}

Struct::~Struct() {
}

Struct::Struct(std::string type_name, std::weak_ptr<Type> parent, std::vector<std::string> arguments) :
    Type{type_name, parent, arguments} {
}

bool operator==(const Struct& lhs, const Struct& rhs) {
    return lhs.isEqual(rhs)
        and std::equal(lhs.subtypes.begin(), lhs.subtypes.end(), rhs.subtypes.begin(),
                [](std::shared_ptr<Type> lhsp, std::shared_ptr<Type> rhsp) { return *lhsp == *rhsp; });
}

bool operator!=(const Struct& lhs, const Struct& rhs) {
    return not (lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const Struct& obj) {
    using namespace std::literals::string_literals;
    return os << ":"s << obj.type_name;
}

void Struct::apply(BinaryReader& reader, DataArray& data) {
    for(const auto child : declared) {
        const auto count = data.resolveArgument(child.count);
        for (const auto index : boost::irange(0, count)) {
            DataArray next{
                child.identifier,child.type_name(), index, data.applied_arguments, &data, false};
            next.resolveArguments(child.type.lock()->arguments,child.appliedArguments);
            child.type.lock()->apply(reader, next);
            data.children.push_back(next);
        }
    }
}

std::shared_ptr<const Type> Struct::findSubtype(std::vector<std::string>::const_iterator it_begin,
        std::vector<std::string>::const_iterator it_end) const {
    if (it_begin == it_end)
        return shared_from_this();

    const auto it = std::find_if(subtypes.begin(), subtypes.end(),
            std::bind(typename_equality, _1, *it_begin));
    if (it != subtypes.end())
        return (*it)->climbDown(it_begin + 1, it_end);
    const auto found = climbUp(*it_begin);
    if (found)
        return found->climbDown(it_begin + 1, it_end);

    return nullptr;
}

std::shared_ptr<Type> Struct::findDirectSubtype(const std::string& type_name) const {
    const auto it = std::find_if(subtypes.begin(), subtypes.end(),
            std::bind(typename_equality, _1, type_name)) ;
    return it == subtypes.end()
        ? nullptr
        : *it;
}

std::shared_ptr<const Type> Struct::climbUp(const std::string& where) const {
    auto p = parent.lock();
    for (; p and p->type_name != where; p = p->parent.lock()) {
        const auto it = p->findDirectSubtype(where);
        if (it != nullptr)
            return it;
    }

    return p;
}

std::shared_ptr<const Type> Struct::climbDown(std::vector<std::string>::const_iterator it_begin,
            std::vector<std::string>::const_iterator it_end) const {
    if (it_begin == it_end)
        return shared_from_this();

    const auto it = findDirectSubtype(*it_begin);
    if (it != nullptr)
        return it->climbDown(it_begin + 1, it_end);

    return nullptr;
}

void Struct::addSubtype(std::shared_ptr<Type> subtype) {
    subtypes.push_back(subtype);
}
