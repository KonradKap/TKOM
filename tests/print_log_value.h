#pragma once

#include "print_container.h"

namespace boost {
namespace test_tools {
namespace tt_detail {
    template<class T>
    struct print_log_value<std::vector<T>> {
        void operator()(std::ostream& ostr, const std::vector<T>& input) {
            print(input, ostr);
        }
    };
}
}
}
