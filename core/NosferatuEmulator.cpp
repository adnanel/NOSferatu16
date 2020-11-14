//
// Created by adnan on 13/11/2020.
//

#include <iostream>
#include "NosferatuEmulator.h"
#include "Operations.h"

NosferatuEmulator::NosferatuEmulator() : PC(regs[15]) {
    memory[MemSize - 10] = 0xFF;
    memory[MemSize - 15] = 0xFFFF;
}

const u16 *NosferatuEmulator::getMemory() const {
    return memory;
}

int NosferatuEmulator::step() {
    if (PC >= CodeSize) {
        // todo temp
        return 1;
    }

    const auto &i = codeMemory[PC];
    auto cycleDuration = Operations::OpMap[i.op](i, this);

    ++ PC;
    this->cycles += cycleDuration;

    return cycleDuration;
}

u16 NosferatuEmulator::getRegUnsigned(u4 address) const {
    address &= Mask4;
    auto res = regs[address];
    res &= Mask16;
    return res;
}

s16 NosferatuEmulator::getRegSigned(u4 address) const {
    address &= Mask4;
    auto res = regs[address];
    return adjustSign(res);
}

void NosferatuEmulator::setRegUnsigned(u4 address, u16 value) {
    regs[address & Mask4] = value & Mask16;
}

void NosferatuEmulator::setRegSigned(u4 address, s16 value) {
    if (value < 0) {
        value |= 0x8000;
    }
    regs[address & Mask4] = value & Mask16;
}

u16 NosferatuEmulator::getPC() const {
    return PC;
}

void NosferatuEmulator::setPC(u16 value) {
    PC = value & Mask16;
}

u16 NosferatuEmulator::getMemoryValueUnsigned(u16 address) const {
    return memory[address & Mask16] & Mask16;
}

void NosferatuEmulator::setMemoryValueUnsigned(u16 address, u16 value) {
    address &= Mask16;
    if (address < 0x1000) {
        std::cout<<"Write to ROM attempted!" << address << " = " << value << ". Ignoring... " << std::endl;
        return;
    }

    memory[address & Mask16] = value & Mask16;
}

s16 NosferatuEmulator::getMemoryValueSigned(u16 address) const {
    address &= Mask16;
    auto res = memory[address];
    return adjustSign(res);
}

s16 NosferatuEmulator::adjustSign(u16 value) {
    u16 sign = value & 0x8000;
    value &= 0x7FFF;

    s16 res = sign ? -value : value;
    return res;
}

void NosferatuEmulator::setMemoryValueSigned(u16 address, s16 value) {
    if (value < 0) {
        value |= 0x8000;
    }
    memory[address & Mask16] = value & Mask16;
}

long long int NosferatuEmulator::getCycles() const {
    return cycles;
}
