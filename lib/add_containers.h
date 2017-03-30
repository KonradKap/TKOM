 /*
  * Utility.h
  *
  * 12.10.2016 r.
  *
  * Memotosh
  *
  */

#pragma once

#include <utility>

#include "is_container.h"

template<class Container, typename std::enable_if<is_container<Container>::value>::type* = nullptr >
    Container operator+(Container lhs, const Container& rhs);

template<class Container, typename std::enable_if<is_container<Container>::value>::type* = nullptr >
    Container operator+=(Container& lhs, const Container& rhs);

template<class Container, typename std::enable_if<is_container<Container>::value>::type* = nullptr >
    Container operator+(Container lhs, Container&& rhs);

template<class Container, typename std::enable_if<is_container<Container>::value>::type* = nullptr >
    Container operator+=(Container& lhs, Container&& rhs);

template<class Container, typename std::enable_if<is_container<Container>::value>::type* = nullptr >
    Container operator+(Container lhs, typename Container::value_type element);

template<class Container, typename std::enable_if<is_container<Container>::value>::type* = nullptr >
    Container operator+=(Container& lhs, typename Container::value_type element);

#include "add_containers.hpp"
