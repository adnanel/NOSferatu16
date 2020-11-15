//
// Created by adnan on 15/11/2020.
//

#include "ProgramReader.h"

ProgramReader::ProgramReader(std::istream &input) : input(input) {}

ProgramReader::~ProgramReader() {

}

ProgramReader::operator bool() {
    return static_cast<bool>(input);
}

