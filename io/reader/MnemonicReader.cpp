//
// Created by adnan on 15/11/2020.
//

#include <iostream>
#include "MnemonicReader.h"
#include "../../core/Instruction.h"

u16 MnemonicReader::nextWord() {
    std::string line;
    std::getline(input, line);

    if (*this) {
        return -1;
    }

    trim(line);
    toUpper(line);

    if (line.empty()) {
        return nextWord();
    }
    // comments start with #
    if (line[0] == '#') {
        return nextWord();
    }

    std::string opcode;
    std::string dest;
    std::string opA;
    std::string opB;

    std::string *tokens[] = {&opcode, &dest, &opA, &opB};

    Instruction instruction(0);
    const InstructionMeta* currentMeta = nullptr;

    for (int i = 0; i < 4; ++i) {
        if (!hasToken(line)) {
            std::cerr << "Token not found when expected!" << std::endl;
            return -1;
        }
        *tokens[i] = nextToken(line);

        if (i == 0) {
            currentMeta = lookupInstruction(*tokens[i]);
            if (!currentMeta) {
                std::cerr << "Unknown instruction! " << tokens[i] << std::endl;
                return -1;
            }
        } else if (i == 1) {

        }
    }

    return instruction.opcode();
}

void MnemonicReader::trim(std::string &str) {
    for (int i = 0; i < str.size(); ++i) {
        char c = str[i];
        if (c != ' ' && c != '\n' && c != '\r') {
            break;
        }
        str.erase(i);
    }
    for (int i = str.size() - 1; i >= 0; --i) {
        char c = str[i];
        if (c != ' ' && c != '\n' && c != '\r') {
            break;
        }
        str.erase(i);
    }
}

void MnemonicReader::toUpper(std::string &str) {
    for (char &c : str) {
        c = std::toupper(c);
    }
}

bool MnemonicReader::hasToken(const std::string &line) const {
    return false; // todo
}

std::string MnemonicReader::nextToken(std::string &line) const {
    return std::__cxx11::string(); // todo
}

const InstructionMeta *MnemonicReader::lookupInstruction(const std::string &instruction) const {
    return nullptr; // todo
}
