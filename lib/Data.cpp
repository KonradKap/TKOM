#include "Data.h"

#include <boost/range/irange.hpp>

#include "utility.h"

const Type* Type::findTypeByName(auto it_begin, auto it_end) const {
    return findTypeBy<typename_equality>(it_begin, it_end);
}

Type* Type::findTypeByName(auto it_begin, auto it_end) {
    return const_cast<Type*>(static_cast<const Type*>(this)->findTypeByName(it_begin, it_end));
}

const Type* Type::findTypeByName(const std::vector<std::string>& name) const {
    return findTypeByName(name.begin(), name.end());
}

Type* Type::findTypeByName(const std::vector<std::string>& type_name) {
    return const_cast<Type*>(static_cast<const Type*>(this)->findTypeByName(type_name));
}

bool Type::wasDeclared(const std::string& type_name) const {
    return findTypeByName({type_name}) != nullptr;
}

bool Type::wasUsed(const std::string& identifier) const {
    return std::find_if(types.begin(), types.end(), identifier_equality(identifier)) != types.end()
        and std::find_if(primitives.begin(), primitives.end(), identifier_equality(identifier)) != primitives.end();
}

const Type* Type::findTypeByIdentifier(const std::vector<std::string>& identifier) const {
    return findTypeByIdentifier(identifier.begin(), identifier.end());
}

Type* Type::findTypeByIdentifier(const std::vector<std::string>& identifier) {
    return const_cast<Type*>(static_cast<const Type*>(this)->findTypeByIdentifier(identifier));
}

const Type* Type::findTypeByIdentifier(auto it_begin, auto it_end) const {
    return findTypeBy<identifier_equality>(it_begin, it_end);
}

Type* Type::findTypeByIdentifier(auto it_begin, auto it_end) {
    return const_cast<Type*>(static_cast<const Type*>(this)->findTypeByIdentifier(it_begin, it_end));
}

const Primitive* Type::findPrimitiveByIdentifier(const std::vector<std::string>& identifier) const {
    return findPrimitiveByIdentifier(identifier.begin(), identifier.end());
}

const Primitive* Type::findPrimitiveByIdentifier(auto it_begin, auto it_end) const {
    const auto* parent = findTypeByIdentifier(it_begin, it_end - 1);
    if (not parent)
        return nullptr;
    const auto it = std::find_if(parent->primitives.begin(), parent->primitives.end(), identifier_equality(*(it_end - 1)));
    if (it != parent->primitives.end())
        return std::addressof(*it);
    return nullptr;
}

const Type* Type::next(unsigned count) const {
    auto it = std::find(parent->types.begin(), parent->types.end(), *this);
    if (it == parent->types.end())
        throw std::runtime_error("Types' tree lost its integrity");
    for (const auto _ : boost::irange(0u, count)) {
        std::ignore = _;
        ++it;
        if (it == parent->types.end() or it->type_name != type_name)
            return nullptr;
    }

    return std::addressof(*it);
}

Type* Type::next(unsigned count) {
    return const_cast<Type*>(static_cast<const Type*>(this)->next(count));
}

const Type* Type::nextArray() const {
    auto it = std::find(parent->types.begin(), parent->types.end(), *this);
    if (it == parent->types.end())
        throw std::runtime_error("Types' tree lost its integrity");
    while (++it != parent->types.end() and it->type_name == type_name);
    if (it == parent->types.end())
        return nullptr;

    return std::addressof(*it);
}

Type* Type::nextArray() {
    return const_cast<Type*>(static_cast<const Type*>(this)->nextArray());
}

void Type::for_each(std::function<void (Type&)> on_type, std::function<void (Primitive&)> on_primitive) {
    auto prim = primitives.begin();
    auto type = types.begin();
    for (unsigned i = 0; i < primitives.size() + types.size(); ++i) {
        if (prim != primitives.end() and prim->position == i) {
            on_primitive(*prim);
            ++prim;
        }
        else {
            on_type(*type);
            type->for_each(on_type, on_primitive);
            ++type;
        }
    }
}
