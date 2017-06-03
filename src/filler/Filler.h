#pragma once

#include <string>
#include <list>
#include <memory>

#include "filler/DataArray.h"
#include "parser/ReferenceList.h"
#include "parser/DeclaredData.h"

class BinaryReader;
class Type;

class Filler {
    public:
        Filler(BinaryReader& reader, std::shared_ptr<Type> root);

        DataArray fill();
        void fillNode(const DeclaredData& declared, DataArray& context);

        BinaryReader& reader;
        std::shared_ptr<Type> root;
};
