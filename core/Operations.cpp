//
// Created by adnan on 13/11/2020.
//

#include <iostream>
#include "Operations.h"

OperationImplementation Operations::OpMap[16] = {
        Operations::op0x0,
        Operations::op0x1,
        Operations::op0x2,
        Operations::op0x3,
        Operations::op0x4,
        Operations::op0x5,
        Operations::op0x6,
        Operations::op0x7,
        Operations::op0x8,
        Operations::op0x9,
        Operations::op0xA,
        Operations::op0xB,
        Operations::op0xC,
        Operations::op0xD,
        Operations::op0xE,
        Operations::op0xF};

void Operations::op0x0(const Instruction &instruction, NosferatuEmulator *emu) {
    emu->setRegUnsigned(instruction.dest, emu->getMemoryValueUnsigned(emu->getRegUnsigned(instruction.opB)));
}

void Operations::op0x1(const Instruction &instruction, NosferatuEmulator *emu) {
    emu->setRegUnsigned(instruction.dest, emu->getRegUnsigned(instruction.opA) + emu->getRegUnsigned(instruction.opB));
}

void Operations::op0x2(const Instruction &instruction, NosferatuEmulator *emu) {
    emu->setRegUnsigned(instruction.dest, emu->getRegUnsigned(instruction.opA) - emu->getRegUnsigned(instruction.opB));
}

void Operations::op0x3(const Instruction &instruction, NosferatuEmulator *emu) {
    emu->setRegUnsigned(instruction.dest, emu->getRegUnsigned(instruction.opA) & emu->getRegUnsigned(instruction.opB));
}

void Operations::op0x4(const Instruction &instruction, NosferatuEmulator *emu) {
    emu->setRegUnsigned(instruction.dest, emu->getRegUnsigned(instruction.opA) | emu->getRegUnsigned(instruction.opB));
}

void Operations::op0x5(const Instruction &instruction, NosferatuEmulator *emu) {
    emu->setRegUnsigned(instruction.dest, emu->getRegUnsigned(instruction.opA) ^ emu->getRegUnsigned(instruction.opB));
}

void Operations::op0x6(const Instruction &instruction, NosferatuEmulator *emu) {
    u16 r3 = emu->getRegUnsigned(instruction.opB);
    u16 r2 = emu->getRegUnsigned(instruction.opA);


    u4 shift = r3 & 0b1111;
    u4 mode = (r3 & 0b110000) >> 4;

    u16 result;
    s16 r2Signed = r2;

    switch (mode) {
        case 0:
            r2Signed >>= shift;
            result = static_cast<u16>(r2Signed);
            break;
        case 1:
            result = r2 >> shift;
            break;
        case 2:
            result = r2 << shift;
            break;
        case 3:
            // rotate left
            u16 rotated = r2 >> (16 - shift);
            result = r2 << shift;
            result |= rotated;
            break;
    }

    emu->setRegUnsigned(instruction.dest, result);
}

void Operations::op0x7(const Instruction &instruction, NosferatuEmulator *emu) {
    emu->setRegUnsigned(instruction.dest, emu->getRegUnsigned(instruction.opA) * emu->getRegUnsigned(instruction.opB));
}

void Operations::op0x8(const Instruction &instruction, NosferatuEmulator *emu) {
    auto val = emu->getRegUnsigned(instruction.opA);
    emu->setMemoryValueUnsigned(emu->getRegUnsigned(instruction.opB), val);
    emu->setRegUnsigned(instruction.dest, val);
}

void Operations::op0x9(const Instruction &instruction, NosferatuEmulator *emu) {
    auto a = instruction.opA;
    auto b = instruction.opB;
    u16 result = ((a & Mask4) << 4) | (b & Mask4);

    emu->setRegUnsigned(instruction.dest, result);
}

void Operations::op0xA(const Instruction &instruction, NosferatuEmulator *emu) {
    auto a = emu->getRegUnsigned(instruction.opA);
    auto b = emu->getRegUnsigned(instruction.opB);
    emu->setRegUnsigned(instruction.dest, a > b ? 1 : 0);
}

void Operations::op0xB(const Instruction &instruction, NosferatuEmulator *emu) {
    auto a = emu->getRegSigned(instruction.opA);
    auto b = emu->getRegSigned(instruction.opB);
    emu->setRegUnsigned(instruction.dest, a > b ? 1 : 0);
}

void Operations::op0xC(const Instruction &instruction, NosferatuEmulator *emu) {
    auto a = emu->getRegUnsigned(instruction.opA);
    auto b = emu->getRegUnsigned(instruction.opB);
    emu->setRegUnsigned(instruction.dest, a < b ? 1 : 0);
}

void Operations::op0xD(const Instruction &instruction, NosferatuEmulator *emu) {
    auto a = emu->getRegSigned(instruction.opA);
    auto b = emu->getRegSigned(instruction.opB);
    emu->setRegUnsigned(instruction.dest, a < b ? 1 : 0);
}

void Operations::op0xE(const Instruction &instruction, NosferatuEmulator *emu) {
    auto a = emu->getRegUnsigned(instruction.opA);
    auto b = emu->getRegUnsigned(instruction.opB);
    emu->setRegUnsigned(instruction.dest, a == b ? 1 : 0);
}

void Operations::op0xF(const Instruction &instruction, NosferatuEmulator *emu) {
    auto a = emu->getRegUnsigned(instruction.opA);
    auto b = emu->getRegUnsigned(instruction.opB);
    emu->setRegUnsigned(instruction.dest, a);
    emu->setPC(b);
}

