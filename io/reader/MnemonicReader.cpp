//
// Created by adnan on 15/11/2020.
//

#include <iostream>
#include <cstring>
#include "MnemonicReader.h"
#include "../../core/Instruction.h"

u16 MnemonicReader::nextWord() {
    std::string line;
    std::getline(input, line);

    if (!*this) {
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
    const InstructionMeta *currentMeta = nullptr;

    for (int i = 0; i < 4; ++i) {
        if (!hasToken(line)) {
            if (currentMeta && currentMeta->argCount == i - 1) {
                break;
            }
            std::cerr << "Token not found when expected! " << line << std::endl;
            return -1;
        }
        *tokens[i] = nextToken(line);

        if (i == 0) {
            currentMeta = lookupInstruction(*tokens[i]);
            if (!currentMeta) {
                std::cerr << "Unknown instruction! " << tokens[i] << std::endl;
                return -1;
            }
        }
    }

    if (!currentMeta) {
        std::cerr << "No instruction detected!" << std::endl;
        return -1;
    }
    instruction.op = currentMeta->op;
    instruction.dest = parseNumber4(dest);
    if (currentMeta->argCount == 2) {
        // immediate value
        u16 immediate = parseNumber8(opA);
        instruction.opA = (immediate >> 4) & Mask4;
        instruction.opB = immediate & Mask4;
    } else {
        instruction.opA = parseNumber4(opA);
        instruction.opB = parseNumber4(opB);
    }

    return instruction.opcode();
}

void MnemonicReader::trim(std::string &str) const {
    for (int i = 0; i < str.size(); ++i) {
        char c = str[i];
        if (c != ' ' && c != '\n' && c != '\r') {
            break;
        }
        str.erase(i, 1);
    }
    for (int i = str.size() - 1; i >= 0; --i) {
        char c = str[i];
        if (c != ' ' && c != '\n' && c != '\r') {
            break;
        }
        str.erase(i, 1);
    }
}

void MnemonicReader::toUpper(std::string &str) const {
    for (char &c : str) {
        c = std::toupper(c);
    }
}

bool MnemonicReader::hasToken(const std::string &line) const {
    return !line.empty() && std::isalnum(line[0]);
}

std::string MnemonicReader::nextToken(std::string &line) {
    std::string token;
    seekToNextToken(line);
    while (!line.empty() && std::isalnum(line[0])) {
        token += line[0];
        line.erase(0, 1);
    }
    seekToNextToken(line);
    return token;
}

const InstructionMeta *MnemonicReader::lookupInstruction(const std::string &instruction) const {
    for (const auto &curr : mnemonics) {
        if (curr.mnemonic == instruction) {
            return &curr;
        }
    }
    return nullptr;
}

void MnemonicReader::seekToNextToken(std::string &line) {
    while (!line.empty() && !std::isalnum(line[0])) {
        line.erase(0, 1);
    }
}

MnemonicReader::MnemonicReader(std::istream &input) : ProgramReader(input) {}

u4 MnemonicReader::parseNumber4(std::string str) const {
    toUpper(str);
    unsigned int result;

    if (str.find('X') == std::string::npos) {
        // decimal number
        sscanf(str.c_str(), "%d", &result);
    } else {
        // hexadecimal number
        sscanf(str.c_str(), "%x", &result);
    }

    return result & Mask4;
}

u8 MnemonicReader::parseNumber8(std::string str) const {
    toUpper(str);
    unsigned int result;

    if (str.find('X') == std::string::npos) {
        // decimal number
        sscanf(str.c_str(), "%d", &result);
    } else {
        // hexadecimal number
        sscanf(str.c_str(), "%x", &result);
    }

    return result & Mask8;
}
