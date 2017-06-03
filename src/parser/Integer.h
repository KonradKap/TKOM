#pragma once

#include <vector>

#include "Type.h"
#include "ReferenceList.h"

class Integer : public Type {
    public:
        Integer(std::weak_ptr<Type> parent);
        virtual ~Integer();

        virtual std::vector<ReferenceList> defaultArguments() const;

        virtual void apply(BinaryReader& reader, DataArray& data);

        virtual std::shared_ptr<const Type> findSubtype(std::vector<std::string>::const_iterator it_begin,
                        std::vector<std::string>::const_iterator it_end) const;

        virtual std::shared_ptr<Type> findDirectSubtype(const std::string& type_name) const;

        virtual std::shared_ptr<const Type> climbDown(std::vector<std::string>::const_iterator it_begin,
                        std::vector<std::string>::const_iterator it_end) const;

        const static std::vector<ReferenceList> defaultSize;
};
