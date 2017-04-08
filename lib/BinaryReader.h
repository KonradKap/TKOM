#pragma once

#include <fstream>
#include <cstdint>
#include <memory>

class BinaryReader {
    public:
        ///It is actually very important to pass istream opened with ios::binary
        ///VERY important
        BinaryReader(std::unique_ptr<std::istream> input);

        int64_t read(unsigned bit_count);

        unsigned getOffset() const;
        std::streampos getPosition();

        void setOffset(unsigned new_offset);
        void setPosition(std::streampos new_position);
    private:
        unsigned offset;
        std::unique_ptr<std::istream> input;
};
