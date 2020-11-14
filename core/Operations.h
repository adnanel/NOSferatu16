//
// Created by adnan on 13/11/2020.
//

#ifndef NOSFERATU16_OPERATIONS_H
#define NOSFERATU16_OPERATIONS_H

#include "Instruction.h"
#include "NosferatuEmulator.h"

#include <functional>

typedef std::function<void(const Instruction&, NosferatuEmulator*)> OperationImplementation;

class Operations {
private:
    Operations() = default;
    Operations(const Operations& op) = default;
    Operations(Operations&& op) = default;
public:
    static OperationImplementation OpMap[];
    
    
    //  LOD R1,R2,(R3)
    static void op0x0(const Instruction& instruction, NosferatuEmulator* emu);
    // ADD R1,R2,R3
    static void op0x1(const Instruction& instruction, NosferatuEmulator* emu);
    // SUB R1,R2,R3
    static void op0x2(const Instruction& instruction, NosferatuEmulator* emu);
    // AND R1,R2,R3
    static void op0x3(const Instruction& instruction, NosferatuEmulator* emu);
    // ORA R1,R2,R3
    static void op0x4(const Instruction& instruction, NosferatuEmulator* emu);
    // XOR R1,R2,R3
    static void op0x5(const Instruction& instruction, NosferatuEmulator* emu);
    // SHR R1,R2,R3
    static void op0x6(const Instruction& instruction, NosferatuEmulator* emu);
    // MUL R1,R2,R3
    static void op0x7(const Instruction& instruction, NosferatuEmulator* emu);
    // STO R1,R2,(R3)
    static void op0x8(const Instruction& instruction, NosferatuEmulator* emu);
    // LDC R1,22
    static void op0x9(const Instruction& instruction, NosferatuEmulator* emu);
    // GTU R1,R2,R3
    static void op0xA(const Instruction& instruction, NosferatuEmulator* emu);
    // GTS R1,R2,R3
    static void op0xB(const Instruction& instruction, NosferatuEmulator* emu);
    // LTU R1,R2,R3
    static void op0xC(const Instruction& instruction, NosferatuEmulator* emu);
    // LTS R1,R2,R3
    static void op0xD(const Instruction& instruction, NosferatuEmulator* emu);
    // EQU R1,R2,R3
    static void op0xE(const Instruction& instruction, NosferatuEmulator* emu);
    // MAJ R1,R2,R3
    static void op0xF(const Instruction& instruction, NosferatuEmulator* emu);
};


#endif //NOSFERATU16_OPERATIONS_H
