//
// Created by adnan on 13/11/2020.
//

#ifndef NOSFERATU16_NOSFERATUEMULATOR_H
#define NOSFERATU16_NOSFERATUEMULATOR_H

#include "../types.h"
#include "Instruction.h"

constexpr int MemSize = 65536;
constexpr int CodeSize = 2;
constexpr int RegCount = 16;


class NosferatuEmulator {
private:

    Instruction* codeMemory[MemSize] = { nullptr };
    unsigned long cachedInstructionCount = 0;

    u16 memory[MemSize] = {0};
    u16 regs[RegCount] = {0};

    u16& PC;

    long long cycles = 0;

    static s16 adjustSign(u16 value);
public:
    NosferatuEmulator();

    const u16* getMemory() const;

    void step();

    long long int getCycles() const;

    u16 getRegUnsigned(u4 address) const;
    void setRegUnsigned(u4 address, u16 value);

    s16 getRegSigned(u4 address) const;
    void setRegSigned(u4 address, s16 value);

    u16 getMemoryValueUnsigned(u16 address) const;
    void setMemoryValueUnsigned(u16 address, u16 value);

    s16 getMemoryValueSigned(u16 address) const;
    void setMemoryValueSigned(u16 address, s16 value);

    const u16* getVideoMemory() const;

    u16 getPC() const;
    void setPC(u16 value);

    const Instruction* readInstruction(u16 address);

    unsigned long getCachedInstructionCount() const;
};


#endif //NOSFERATU16_NOSFERATUEMULATOR_H
