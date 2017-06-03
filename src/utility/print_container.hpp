#include <sstream>

template<class Container, typename std::enable_if<boost::spirit::traits::is_container<Container>::value>::type* >
    void print(const Container& input, std::ostream& out, std::string separator) {
        std::ostream_iterator<typename Container::value_type> out_iterator{out, separator.c_str()};
        std::copy(std::begin(input), std::end(input)-1, out_iterator);
        if(input.size())
            out << input.back();
    }

template<class Container, typename std::enable_if<boost::spirit::traits::is_container<Container>::value>::type* >
    std::string toString(const Container& input, std::string separator) {
        std::stringstream ss;
        print(input, ss, separator);
        return ss.str();
    }
