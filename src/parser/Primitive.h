#pragma once

#include <string>
#include <vector>

struct Primitive {
    std::string identifier;
    
    //unsigned size;
    //std::vector<int64_t> value;
    //unsigned position;

    std::string toString() const;
};

bool operator==(const Primitive& lhs, const Primitive& rhs);
bool operator!=(const Primitive& lhs, const Primitive& rhs);
