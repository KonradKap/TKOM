#include "utility/utility.h"

const Type* Type::climbDown(auto it_begin, auto it_end) const {
    if (it_begin == it_end)
        return this;

    const auto it = std::find_if(subtypes.begin(), subtypes.end(),
            [it_begin](const Type& type) { return type.type_name == *it_begin; });
    if (it != subtypes.end())
        return it->climbDown(it_begin + 1, it_end);

    return nullptr;
}

//template<class Equality>
//    const Type* Type::findTypeBy(auto it_begin, auto it_end) const {
//        if (it_begin == it_end)
//            return this;
//
//        const auto it = std::find_if(subtypes.begin(), subtypes.end(), Equality(*it_begin));
//        if (it != types.end())
//            return (*it)->climbDown<Equality>(it_begin + 1, it_end);
//
//        const auto found = climbUp<Equality>(*it_begin);
//        if (found)
//            return found->climbDown<Equality>(it_begin + 1, it_end);
//
//        return nullptr;
//    }

const Type* Type::findSubtype(auto it_begin, auto it_end) const {
    if (it_begin == it_end)
        return this;

    const auto it = std::find_if(subtypes.begin(), subtypes.end(),
            [it_begin](const Type* type) { return type->type_name == *it_begin; });
    if (it != subtypes.end())
        return it->climbDown(it_begin + 1, it_end);
    const auto found = climbUp(*it_begin);
    if (found)
        return found->climbDown(it_begin + 1, it_end);

    return nullptr;
}

Type* Type::findSubtype(auto it_begin, auto it_end) {
    return const_cast<Type*>(static_cast<const Type*>(this)->findSubtype(it_begin, it_end));
}

//const Type* Type::findTypeByIdentifier(auto it_begin, auto it_end) const {
//    return findTypeBy<identifier_equality>(it_begin, it_end);
//}
//
//Type* Type::findTypeByIdentifier(auto it_begin, auto it_end) {
//    return const_cast<Type*>(static_cast<const Type*>(this)->findTypeByIdentifier(it_begin, it_end));
//}
//
//const Primitive* Type::findPrimitiveByIdentifier(auto it_begin, auto it_end) const {
//    const auto* parent = findTypeByIdentifier(it_begin, it_end - 1);
//    if (not parent)
//        return nullptr;
//    const auto it = std::find_if(parent->primitives.begin(), parent->primitives.end(), identifier_equality(*(it_end - 1)));
//    if (it != parent->primitives.end())
//        return std::addressof(*it);
//    return nullptr;
//}
//
