//
// Created by adnan on 14/11/2020.
//

#ifndef NOSFERATU16_FREQUENCYCALCULATOR_H
#define NOSFERATU16_FREQUENCYCALCULATOR_H

#include <chrono>

class FrequencyCalculator {
    std::chrono::time_point<std::chrono::high_resolution_clock> timer = std::chrono::high_resolution_clock::now();
    long cycles = 0;
    long frequency = 0;
public:
    void step() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>( now - timer ).count();
        if (duration >= 1000000) {
            // second has passed, refresh the frequency
            frequency = cycles;
            cycles = 0;
            timer = now;
        }
    }

    inline void addCycles(int cycles) {
        this->cycles += cycles;
    }
    inline long getFrequency() const {
        return frequency;
    }
    inline long getCycles() const {
        return this->cycles;
    }
};


#endif //NOSFERATU16_FREQUENCYCALCULATOR_H
