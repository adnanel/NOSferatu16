//
// Created by adnan on 15/11/2020.
//

#ifndef NOSFERATU16_PROGRAMCONVERTER_H
#define NOSFERATU16_PROGRAMCONVERTER_H


#include "../reader/ProgramReader.h"

class ProgramConverter {
private:
    ProgramReader* source;
    std::ostream& output;
public:
    ProgramConverter(ProgramReader *source, std::ostream &output);

    void convert();
};


#endif //NOSFERATU16_PROGRAMCONVERTER_H
