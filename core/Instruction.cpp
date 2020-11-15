//
// Created by adnan on 13/11/2020.
//

#include "Instruction.h"

Instruction::Instruction(u16 opcode) {
    opcode &= Mask16;

    op = opcode >> 12;
    dest = (opcode >> 8) & Mask4;
    opA = (opcode >> 4) & Mask4;
    opB = opcode & Mask4;
}

u16 Instruction::opcode() const {
    u16 result = op & Mask4;
    result <<= 4u;
    result |= dest & Mask4;

    result <<= 4u;
    result |= opA & Mask4;

    result <<= 4u;
    result |= opB & Mask4;

    return result;
}
