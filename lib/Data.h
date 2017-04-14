#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>

#include "Scanner.h"

struct Primitive {
    std::string identifier;
    unsigned size;
    std::vector<int64_t> value;
    unsigned position;
};

struct Type {
    std::string type_name = "";
    std::string identifier = "";
    Type* parent = nullptr;

    std::list<Type> types = {};
    std::vector<Primitive> primitives = {};

    Scanner::saved_pos declaration;

    const Type* findTypeByName(const std::vector<std::string>& type_name) const;
    Type* findTypeByName(const std::vector<std::string>& type_name);

    const Type* findTypeByName(auto it_begin, auto it_end) const;
    Type* findTypeByName(auto it_begin, auto it_end);

    const Type* findTypeByIdentifier(const std::vector<std::string>& identifier) const;
    Type* findTypeByIdentifier(const std::vector<std::string>& identifier);

    const Type* findTypeByIdentifier(auto it_begin, auto it_end) const;
    Type* findTypeByIdentifier(auto it_begin, auto it_end);

    bool wasDeclared(const std::string& type_name) const;
    bool wasUsed(const std::string& identifier) const;

    const Type* next(unsigned count) const;
    Type* next(unsigned count);

    const Type* nextArray() const;
    Type* nextArray();

    template<class Equality>
        const Type* findTypeBy(auto it_begin, auto it_end) const;
    template<class Equality>
        const Type* climbDown(auto it_begin, auto it_end) const;
    template<class Equality>
        const Type* climbUp(const std::string& where) const;

    const Primitive* findPrimitiveByIdentifier(auto it_begin, auto it_end) const;
    const Primitive* findPrimitiveByIdentifier(const std::vector<std::string>& identifier) const;

    void for_each(std::function<void (Type&)> on_type, std::function<void (Primitive&)> on_primitive);
};

template<class Equality>
    const Type* Type::climbUp(const std::string& where) const {
        auto *p = parent;
        for (; p and p->type_name != where; p = p->parent) {
            const auto it = std::find_if(p->types.begin(), p->types.end(), Equality(where));
            if (it != p->types.end())
                return std::addressof(*it);
        }

        return p;
    }

template<class Equality>
    const Type* Type::climbDown(auto it_begin, auto it_end) const {
        if (it_begin == it_end)
            return this;

        const auto it = std::find_if(types.begin(), types.end(), Equality(*it_begin));
        if (it != types.end())
            return it->climbDown<Equality>(it_begin + 1, it_end);

        return nullptr;
    }

template<class Equality>
    const Type* Type::findTypeBy(auto it_begin, auto it_end) const {
        if (it_begin == it_end)
            return this;

        const auto it = std::find_if(types.begin(), types.end(), Equality(*it_begin));
        if (it != types.end())
            return it->climbDown<Equality>(it_begin + 1, it_end);

        const auto found = climbUp<Equality>(*it_begin);
        if (found)
            return found->climbDown<Equality>(it_begin + 1, it_end);

        return nullptr;
    }

inline bool operator==(const Type& lhs, const Type& rhs) {
    return lhs.type_name == rhs.type_name
        and lhs.identifier == rhs.identifier
        and lhs.types == rhs.types
        and lhs.primitives == rhs.primitives
        and lhs.parent == rhs.parent;
}

inline bool operator!=(const Type& lhs, const Type& rhs) {
    return not (lhs == rhs);
}

inline bool operator==(const Primitive& lhs, const Primitive& rhs) {
    return lhs.size == rhs.size
        and lhs.value == rhs.value
        and lhs.identifier == rhs.identifier
        and lhs.position == rhs.position;
}

inline bool operator!=(const Primitive& lhs, const Primitive& rhs) {
    return not (lhs == rhs);
}
