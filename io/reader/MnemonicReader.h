//
// Created by adnan on 15/11/2020.
//

#ifndef NOSFERATU16_MNEMONICREADER_H
#define NOSFERATU16_MNEMONICREADER_H


#include "ProgramReader.h"

struct InstructionMeta {
    std::string mnemonic;
    int argCount;
};
const InstructionMeta mnemonics[] = {
        { "LOD", 3 },
        { "ADD", 3 },
        { "SUB", 3 },
        { "AND", 3 },
        { "ORA", 3 },
        { "XOR", 3 },
        { "SHR", 3 },
        { "MUL", 3 },
        { "STO", 3 },
        { "LDC", 2 },
        { "GTU", 3 },
        { "GTS", 3 },
        { "LTU", 3 },
        { "LTS", 3 },
        { "EQU", 3 },
        { "MAJ", 3 },
};

class MnemonicReader : public ProgramReader {
private:
    void trim(std::string& str);
    void toUpper(std::string& str);

    bool hasToken(const std::string& line) const;
    std::string nextToken(std::string& line) const;
    const InstructionMeta* lookupInstruction(const std::string& instruction) const;
public:
    u16 nextWord() override;
};


#endif //NOSFERATU16_MNEMONICREADER_H
