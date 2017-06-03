#include "Primitive.h"

using namespace std::literals::string_literals;

std::string Primitive::toString() const {
    //auto str = "int ("s + std::to_string(size) + ")[" + std::to_string(value.size()) + "] " + identifier + " = {";
    //if (value.size() != 0) {
    //    for (auto it = value.cbegin(); it != value.cend() - 1; ++it)
    //        str += std::to_string(*it) + ", ";
    //    str += std::to_string(value.back());
    //}
    //return str + "}";
    return "not implemented";
}

bool operator==(const Primitive& lhs, const Primitive& rhs) {
    return //lhs.size == rhs.size;
        //and lhs.value == rhs.value
        /*and*/ lhs.identifier == rhs.identifier;
        //and lhs.position == rhs.position;
}

bool operator!=(const Primitive& lhs, const Primitive& rhs) {
    return not (lhs == rhs);
}
