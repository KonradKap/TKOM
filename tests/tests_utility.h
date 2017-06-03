#pragma once

#include <string>
#include <vector>

#include "lexer/Scanner.h"
#include "filler/BinaryReader.h"
#include "parser/Parser.h"
#include "filler/DataArray.h"

Scanner scannerFromString(std::string in);
BinaryReader readerFromString(std::string in);
ReferenceList makeShallowList(const std::vector<std::string>& names);
DataArray makeDataArray(std::string identifier, int value);
DataArray makeDataArray(std::string identifier);
