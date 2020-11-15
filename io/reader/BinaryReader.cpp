//
// Created by adnan on 13/11/2020.
//

#include "BinaryReader.h"

u16 BinaryReader::nextWord() {
    u8 c;
    input>>c;

    u16 result = c;
    input>>c;
    result <<= 8u;
    result |= c;

    return result;
}
