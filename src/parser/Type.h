#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>

#include "DeclaredData.h"

typedef std::vector<std::string> argument_list_t;
class DataArray;
class BinaryReader;

class Type : public std::enable_shared_from_this<Type> {
    public:
        std::string type_name = "";
        std::weak_ptr<Type> parent = {};

        std::vector<DeclaredData> declared = {};

        argument_list_t arguments;

        Type();
        Type(std::string type_name, std::weak_ptr<Type> parent, std::vector<std::string> args = {});
        virtual ~Type();

        std::shared_ptr<const Type> findSubtype(const std::vector<std::string>& name) const;
        std::shared_ptr<Type> findSubtype(const std::vector<std::string>& type_name);

        virtual std::shared_ptr<const Type> findSubtype(std::vector<std::string>::const_iterator it_begin,
                        std::vector<std::string>::const_iterator it_end) const = 0;
        std::shared_ptr<Type> findSubtype(std::vector<std::string>::const_iterator it_begin,
                        std::vector<std::string>::const_iterator it_end);

        bool isTypeEqual(const Type& t) const;
        virtual bool isEqual(const Type& t) const;

        virtual std::shared_ptr<Type> findDirectSubtype(const std::string& type_name) const = 0;
        virtual void addSubtype(std::shared_ptr<Type> subtype);
        bool wasDeclared(const std::string& type_name) const;
        bool wasUsed(const std::string& identifier) const;

        argument_list_t::size_type argumentCount() const;
        virtual std::vector<ReferenceList> defaultArguments() const;
        virtual void apply(BinaryReader& reader, DataArray& data) = 0;

        virtual std::shared_ptr<const Type> climbDown(std::vector<std::string>::const_iterator it_begin,
                        std::vector<std::string>::const_iterator it_end) const = 0;
        virtual std::shared_ptr<const Type> climbUp(const std::string& where) const;

        const static std::function<bool (std::shared_ptr<const Type> type, std::string)> typename_equality;
        const static std::function<bool (const DeclaredData& data, std::string)> identifier_equality;
};

bool operator==(const Type& lhs, const Type& rhs);
bool operator!=(const Type& lhs, const Type& rhs);
std::ostream& operator<<(std::ostream& os, const Type& obj);
