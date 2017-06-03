#pragma once

#include <vector>
#include <memory>

#include "Type.h"
#include "ReferenceList.h"
#include "utility/add_containers.h"

class Union : public Type {
    public:
        Union(std::string type_name, std::weak_ptr<Type> parent, argument_list_t list = {});
        virtual ~Union();

        virtual bool isEqual(const Type& t) const;
        std::vector<std::pair<ReferenceList, std::shared_ptr<Type>>> cases;
        std::shared_ptr<Type> default_case;

        std::shared_ptr<Type> getCase(const DataArray& data);
        virtual void apply(BinaryReader& reader, DataArray& data);

        virtual std::shared_ptr<const Type> findSubtype(std::vector<std::string>::const_iterator it_begin,
                        std::vector<std::string>::const_iterator it_end) const;

        virtual std::shared_ptr<Type> findDirectSubtype(const std::string& type_name) const;

        virtual std::shared_ptr<const Type> climbDown(std::vector<std::string>::const_iterator it_begin,
                        std::vector<std::string>::const_iterator it_end) const;
};

bool operator==(const Union& lhs, const Union& rhs);
bool operator!=(const Union& lhs, const Union& rhs);
std::ostream& operator<<(std::ostream& os, const Union& obj);
