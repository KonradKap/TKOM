#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <functional>

struct Primitive {
    unsigned size;
    std::vector<int64_t> value;
    std::string identifier;
    unsigned position;

    Primitive() :
        size(32), value(1, 0), identifier(), position() {}
};

struct Type {
    std::string identifier;
    std::vector<int> types;
    std::vector<Primitive> primitives;
    //unsigned count;

    Type() :
        identifier(), types(), primitives() {}
};
