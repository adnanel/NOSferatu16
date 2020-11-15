//
// Created by adnan on 13/11/2020.
//

#ifndef NOSFERATU16_BINARYREADER_H
#define NOSFERATU16_BINARYREADER_H


#include "ProgramReader.h"

class BinaryReader : public ProgramReader {
public:
    u16 nextWord() override;
};


#endif //NOSFERATU16_BINARYREADER_H
