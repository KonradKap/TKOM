#include "Union.h"

#include "utility/CompilationError.h"
#include "filler/DataArray.h"

Union::Union(std::string type_name, std::weak_ptr<Type> parent, argument_list_t list) :
    Type{type_name, parent, {""}},
    cases{},
    default_case{nullptr} {
        arguments += list;
    }

Union::~Union() {
}

bool Union::isEqual(const Type& t) const {
    if (not isTypeEqual(t))
        return false;
    const Union& obj = static_cast<const Union&>(t);
    if (not std::equal(cases.begin(), cases.end(), obj.cases.begin(),
        [](decltype(cases)::value_type lhsp, decltype(cases)::value_type rhsp)
            { return lhsp.first == rhsp.first and *lhsp.second == *rhsp.second; }))
                return false;
    if (obj.default_case and default_case)
        return *obj.default_case == *default_case;
    return default_case == obj.default_case;
}

bool operator==(const Union& lhs, const Union& rhs) {
    return lhs.isEqual(rhs);
}

bool operator!=(const Union& lhs, const Union& rhs) {
    return not (lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const Union& obj) {
    return os << static_cast<const Type&>(obj);
}

std::shared_ptr<Type> Union::getCase(const DataArray& data) {
    const auto it = std::find_if(cases.begin(), cases.end(),
        [data](const decltype(cases)::value_type& type) {
            return data.resolveArgument(type.first) == data.applied_arguments.begin()->second;
        });

    return it == cases.end()
        ? default_case
        : it->second;
}

void Union::apply(BinaryReader& reader, DataArray& data) {
    const auto ptr = getCase(data);
    if (not ptr)
        throw CompilationError("Didn't find fitting case");

    data.applied_arguments.erase(data.applied_arguments.begin());
    ptr->apply(reader, data);
}

std::shared_ptr<const Type> Union::findSubtype(std::vector<std::string>::const_iterator it_begin,
        std::vector<std::string>::const_iterator it_end) const {

    if (it_begin == it_end)
        return shared_from_this();

    for (const auto& c : cases) {
        const auto subtype = c.second->findSubtype(it_begin, it_end);
        if (subtype != nullptr)
            return subtype;
    }
    if(default_case)
        return default_case->findSubtype(it_begin, it_end);
    return nullptr;
}

std::shared_ptr<Type> Union::findDirectSubtype(const std::string& type_name) const {
    for (const auto& c : cases) {
        const auto subtype = c.second->findDirectSubtype(type_name);
        if (subtype != nullptr)
            return subtype;
    }
    if(default_case)
        return default_case->findDirectSubtype(type_name);
    return nullptr;
}

std::shared_ptr<const Type> Union::climbDown(std::vector<std::string>::const_iterator it_begin,
        std::vector<std::string>::const_iterator it_end) const {

    if (it_begin == it_end)
        return shared_from_this();

    const auto begin = findDirectSubtype(*it_begin);
    if (begin)
        return begin->climbDown(it_begin + 1, it_end);
    return nullptr;
}

