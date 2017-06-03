#include "Integer.h"

#include "filler/Filler.h"
#include "filler/BinaryReader.h"

decltype(Integer::defaultSize) Integer::defaultSize{ReferenceList{}.addElement({{}, "32"})};

Integer::Integer(std::weak_ptr<Type> parent) :
    Type("Integer", parent, {""}) {
}

Integer::~Integer() {
}

std::vector<ReferenceList> Integer::defaultArguments() const {
    return defaultSize;
}

void Integer::apply(BinaryReader& reader, DataArray& data) {
    data.value = reader.read(data.applied_arguments.begin()->second);
    data.has_value = true;
}

std::shared_ptr<const Type> Integer::findSubtype(std::vector<std::string>::const_iterator it_begin,
        std::vector<std::string>::const_iterator it_end) const {

    return nullptr;
}

std::shared_ptr<Type> Integer::findDirectSubtype(const std::string& type_name) const {
    return nullptr;
}

std::shared_ptr<const Type> Integer::climbDown(std::vector<std::string>::const_iterator it_begin,
        std::vector<std::string>::const_iterator it_end) const {

    return nullptr;
}
