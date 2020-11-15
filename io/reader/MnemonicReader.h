//
// Created by adnan on 15/11/2020.
//

#ifndef NOSFERATU16_MNEMONICREADER_H
#define NOSFERATU16_MNEMONICREADER_H


#include "ProgramReader.h"

struct InstructionMeta {
    std::string mnemonic;
    int argCount;
    int op;
};
const InstructionMeta mnemonics[] = {
        { "LOD", 3, 0x0 },
        { "ADD", 3, 0x1 },
        { "SUB", 3, 0x2 },
        { "AND", 3, 0x3 },
        { "ORA", 3, 0x4 },
        { "XOR", 3, 0x5 },
        { "SHR", 3, 0x6 },
        { "MUL", 3, 0x7 },
        { "STO", 3, 0x8 },
        { "LDC", 2, 0x9 },
        { "GTU", 3, 0xA },
        { "GTS", 3, 0xB },
        { "LTU", 3, 0xC },
        { "LTS", 3, 0xD },
        { "EQU", 3, 0xE },
        { "MAJ", 3, 0xF },
};

class MnemonicReader : public ProgramReader {
public:
    MnemonicReader(std::istream &input);

private:
    void trim(std::string& str) const;
    void toUpper(std::string& str) const;

    void seekToNextToken(std::string& str);
    bool hasToken(const std::string& line) const;
    std::string nextToken(std::string& line);
    const InstructionMeta* lookupInstruction(const std::string& instruction) const;

    u4 parseNumber4(std::string str) const;
    u8 parseNumber8(std::string str) const;
public:
    u16 nextWord() override;
};


#endif //NOSFERATU16_MNEMONICREADER_H
