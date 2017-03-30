#pragma once

#include <fstream>
#include <cstdint>

class BinaryReader {
    public:
        BinaryReader(const std::string& filename);

        int64_t read(unsigned bit_count);

        unsigned getOffset() const;
        std::streampos getPosition();

        void setOffset(unsigned new_offset);
        void setPosition(std::streampos new_position);
    private:
        unsigned offset;
        std::ifstream input;
};
