#pragma once

#include <iostream>
#include <string>

#include <boost/spirit/home/support/container.hpp>

template<class Container, typename std::enable_if<boost::spirit::traits::is_container<Container>::value>::type* = nullptr >
    void print(const Container& input, std::ostream& out = std::cout, std::string separator = ", ");

template<class Container, typename std::enable_if<boost::spirit::traits::is_container<Container>::value>::type* = nullptr >
    std::string toString(const Container& input, std::string separator = ", ");

#include "print_container.hpp"
