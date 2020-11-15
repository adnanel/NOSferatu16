//
// Created by adnan on 13/11/2020.
//

#ifndef NOSFERATU16_INSTRUCTION_H
#define NOSFERATU16_INSTRUCTION_H


#include "../types.h"

struct Instruction {
    u4 op;
    u4 dest;
    u4 opA;
    u4 opB;

    Instruction(u16 opcode);

    u16 opcode() const;
};


#endif //NOSFERATU16_INSTRUCTION_H
