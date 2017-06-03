#include "Type.h"

#include <functional>

#include <boost/range/irange.hpp>

using namespace std::placeholders;

decltype(Type::typename_equality) Type::typename_equality
    = [](std::shared_ptr<const Type> type, std::string what) { return type->type_name == what; };

decltype(Type::identifier_equality) Type::identifier_equality
    = [](const DeclaredData& data, std::string what) { return data.identifier == what; };

Type::Type() :
    std::enable_shared_from_this<Type>() {
}

Type::Type(std::string type_name, std::weak_ptr<Type> parent, std::vector<std::string> arguments) :
    std::enable_shared_from_this<Type>(),
    type_name(type_name),
    parent(parent),
    arguments(arguments) {
}

Type::~Type() {
}

std::shared_ptr<const Type> Type::findSubtype(const std::vector<std::string>& name) const {
    return findSubtype(name.begin(), name.end());
}

std::shared_ptr<Type> Type::findSubtype(const std::vector<std::string>& type_name) {
    return std::const_pointer_cast<Type>(static_cast<const Type*>(this)->findSubtype(type_name));
}

std::shared_ptr<Type> Type::findSubtype(std::vector<std::string>::const_iterator it_begin,
            std::vector<std::string>::const_iterator it_end) {
    return std::const_pointer_cast<Type>(static_cast<const Type*>(this)->findSubtype(it_begin, it_end));
}

bool Type::wasDeclared(const std::string& type_name) const {
    return findDirectSubtype(type_name) != nullptr;
}

bool Type::wasUsed(const std::string& identifier) const {
    return wasDeclared(identifier)
        or std::find_if(declared.begin(), declared.end(),
            std::bind(identifier_equality, _1, identifier)) != declared.end();
}

argument_list_t::size_type Type::argumentCount() const {
    return arguments.size();
}

std::vector<ReferenceList> Type::defaultArguments() const {
    return {};
}

bool Type::isTypeEqual(const Type& rhs) const {
    return typeid(*this) == typeid(rhs)
        and type_name == rhs.type_name
        and declared == rhs.declared
        and arguments == rhs.arguments;
}

bool Type::isEqual(const Type& rhs) const {
    return isTypeEqual(rhs);
}

bool operator==(const Type& lhs, const Type& rhs) {
    return lhs.isEqual(rhs);
}

bool operator!=(const Type& lhs, const Type& rhs) {
    return not (lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const Type& obj) {
    return os << obj.type_name;
}

std::shared_ptr<const Type> Type::climbUp(const std::string& where) const {
    if (not parent.expired()) {
        const auto p = parent.lock();
        if (p->type_name == where)
            return p;
        return p->climbUp(where);
    }
    return nullptr;
}

void Type::addSubtype(std::shared_ptr<Type> subtype) {
}
