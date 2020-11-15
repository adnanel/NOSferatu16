//
// Created by adnan on 15/11/2020.
//

#ifndef NOSFERATU16_PROGRAMREADER_H
#define NOSFERATU16_PROGRAMREADER_H


#include <istream>
#include "../../types.h"

class ProgramReader {
protected:
    std::istream input;

public:
    explicit ProgramReader(const std::istream &input);
    virtual ~ProgramReader();

    virtual u16 nextWord() = 0;
    virtual explicit operator bool();
};


#endif //NOSFERATU16_PROGRAMREADER_H
