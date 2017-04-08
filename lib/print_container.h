#pragma once

#include <iostream>

#include <boost/spirit/home/support/container.hpp>

template<class Container, typename std::enable_if<boost::spirit::traits::is_container<Container>::value>::type* = nullptr >
    void print(const Container& input, std::ostream& out = std::cout);

#include "print_container.hpp"
