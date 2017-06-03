#include <utility>
#include <tuple>
#include <cassert>

#include <boost/range/irange.hpp>

template<class Container, typename std::enable_if_t<boost::spirit::traits::is_container<Container>::value>* >
    Container operator+(Container lhs, const Container& rhs) {
        return lhs += rhs;
    }

template<class Container, typename std::enable_if_t<boost::spirit::traits::is_container<Container>::value>* >
    Container& operator+=(Container& lhs, const Container& rhs) {
        lhs.insert(lhs.end(), rhs.begin(), rhs.end());
        return lhs;
    }

template<class Container, typename std::enable_if_t<boost::spirit::traits::is_container<Container>::value>* >
    Container operator+(Container lhs, Container&& rhs) {
        return lhs += rhs;
    }

template<class Container, typename std::enable_if_t<boost::spirit::traits::is_container<Container>::value>* >
    Container& operator+=(Container& lhs, Container&& rhs) {
        lhs.insert(lhs.end(),
            std::make_move_iterator(rhs.begin()),
            std::make_move_iterator(rhs.end()));
        return lhs;
    }

template<class Container, typename std::enable_if_t<boost::spirit::traits::is_container<Container>::value>* >
    Container operator+(Container lhs, typename Container::value_type element) {
        return lhs += element;
    }

template<class Container, typename std::enable_if_t<boost::spirit::traits::is_container<Container>::value>* >
    Container& operator+=(Container& lhs, typename Container::value_type element) {
        lhs.insert(lhs.end(), element);
        return lhs;
    }

template<class Container, typename std::enable_if_t<boost::spirit::traits::is_container<Container>::value>* >
    Container operator*(Container lhs, int rhs) {
        return lhs *= rhs;
    }

template<class Container, typename std::enable_if_t<boost::spirit::traits::is_container<Container>::value>* >
    Container& operator*=(Container& lhs, int rhs) {
        Container source{};
        for (const auto _ : boost::irange(0, rhs)) {
            std::ignore = _;
            source += lhs;
        }
        std::swap(lhs, source);
        return lhs;
    }
