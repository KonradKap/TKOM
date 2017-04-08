#include "BinaryReader.h"

#include <cassert>
#include <iostream>
#include <cstring>
#include <bitset>

BinaryReader::BinaryReader(std::unique_ptr<std::istream> input) :
    offset(0),
    input(std::move(input)) {
}

int64_t BinaryReader::read(unsigned bit_count) {
    if (bit_count == 0)
        return 0;
    const auto is_padding = (offset + bit_count)%8 != 0;
    const auto bytes_to_read = (bit_count + offset) / 8 + (is_padding);

    unsigned char buffer[bytes_to_read];

    memset(buffer, 0, bytes_to_read);
    input->read(reinterpret_cast<char*>(buffer), bytes_to_read);

    uint64_t ret = 0;
    for (unsigned i = 0; i < bytes_to_read; ++i)
        ret |= (uint64_t(buffer[i]) << (64 - (i+1)*8 + offset));

    offset = (bit_count + offset)%8;
    if (is_padding)
        input->seekg(-1, std::ios::cur);

    return (ret >> (64 - bit_count));
}

unsigned BinaryReader::getOffset() const {
    return offset;
}

std::streampos BinaryReader::getPosition() {
    return input->tellg();
}

void BinaryReader::setOffset(unsigned new_offset) {
    offset = new_offset;
}

void BinaryReader::setPosition(std::streampos new_position) {
    input->clear();
    input->seekg(new_position);
}
