//
// Created by adnan on 13/11/2020.
//

#include <iostream>
#include "NosferatuEmulator.h"
#include "Operations.h"

NosferatuEmulator::NosferatuEmulator() : PC(regs[15]) {
}

const u16 *NosferatuEmulator::getMemory() const {
    return memory;
}

void NosferatuEmulator::step() {
    const auto &i = readInstruction(PC);
    Operations::OpMap[i->op](*i, this);

    ++PC;
    ++this->cycles;
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
        std::cout << "Write to ROM attempted!" << address << " = " << value << ". Ignoring... " << std::endl;
        return;
    }

    memory[address] = value & Mask16;
    if (codeMemory[address]) {
        delete codeMemory[address];
        --cachedInstructionCount;
        codeMemory[address] = nullptr;
    }
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

const u16 *NosferatuEmulator::getVideoMemory() const {
    return memory + 0xC000;
}

const Instruction *NosferatuEmulator::readInstruction(u16 address) {
    if (codeMemory[address]) {
        return codeMemory[address];
    }
    ++cachedInstructionCount;
    return codeMemory[address] = new Instruction(getMemoryValueUnsigned(address));
}

unsigned long NosferatuEmulator::getCachedInstructionCount() const {
    return cachedInstructionCount;
}

void NosferatuEmulator::KeyRelease(unsigned int bitoffset) {
    unsigned int wordOffset = bitoffset / 16;
    unsigned int bitOffsetInWord = bitoffset % 16;
    auto km = getKeyboardMemory();

    u16 &word = km[wordOffset];
    u16 mask = 1u << bitOffsetInWord;

    word ^= mask;

    // std::cout<<"resetting "<<wordOffset<<" "<<bitOffsetInWord<<std::endl;
}

void NosferatuEmulator::KeyPress(unsigned int bitoffset) {
    if (bitoffset >= 80) {
        // keyboard keys are mapped from 0xFFF2 to 0xFFF7, which is 5 * 16 = 80 bits
        return;
    }

    unsigned int wordOffset = bitoffset / 16;
    unsigned int bitOffsetInWord = bitoffset % 16;
    auto km = getKeyboardMemory();

    u16 &word = km[wordOffset];
    u16 mask = 1u << bitOffsetInWord;

    word |= mask;

    // std::cout<<"setting "<<wordOffset<<" "<<bitOffsetInWord<<std::endl;
}

const u16 *NosferatuEmulator::getKeyboardMemory() const {
    return memory + 0xFFF2;
}

u16 *NosferatuEmulator::getKeyboardMemory() {
    return memory + 0xFFF2;
}

void NosferatuEmulator::loadProgram(ProgramReader *programReader) {
    int i;
    for (i = 0; i < 0x1000; ++i) {
        if (!*programReader) {
            return;
        }
        memory[i] = programReader->nextWord();
    }
    std::cout << "Read " << i << " opcodes.";
}
