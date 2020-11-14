//
// Created by adnan on 13/11/2020.
//

#ifndef NOSFERATU16_CONFIG_H
#define NOSFERATU16_CONFIG_H

#include <cstdio>
#include <fstream>

struct Config {
    unsigned long clockSpeed;

    static Config FromFile(const char* fpath) {
        std::ifstream istream(fpath);
        //todo
        return {};
    }
};


#endif //NOSFERATU16_CONFIG_H
