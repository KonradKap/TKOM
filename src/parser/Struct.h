#pragma once

#include <string>

#include "Type.h"

class Struct : public Type {
    public:
        std::vector<std::shared_ptr<Type>> subtypes = {};

        Struct();
        Struct(std::string type_name, std::weak_ptr<Type> parent, std::vector<std::string> arguments = {});
        virtual ~Struct();

        virtual std::shared_ptr<const Type> findSubtype(std::vector<std::string>::const_iterator it_begin,
                        std::vector<std::string>::const_iterator it_end) const;

        virtual void apply(BinaryReader& reader, DataArray& data);

        virtual std::shared_ptr<const Type> climbDown(std::vector<std::string>::const_iterator it_begin,
                        std::vector<std::string>::const_iterator it_end) const;

        virtual std::shared_ptr<const Type> climbUp(const std::string& where) const;

        virtual std::shared_ptr<Type> findDirectSubtype(const std::string& type_name) const;

        virtual void addSubtype(std::shared_ptr<Type> subtype);
};

bool operator==(const Struct& lhs, const Struct& rhs);
bool operator!=(const Struct& lhs, const Struct& rhs);
std::ostream& operator<<(std::ostream& os, const Struct& obj);
