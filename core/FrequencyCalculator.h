//
// Created by adnan on 14/11/2020.
//

#ifndef NOSFERATU16_FREQUENCYCALCULATOR_H
#define NOSFERATU16_FREQUENCYCALCULATOR_H

#include <chrono>
#include <iostream>

class FrequencyCalculator {
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();
    unsigned long cycles = 0;

    unsigned long targetFrequency;
    unsigned long calculatedFrequency = 0;

    bool frequencyLocked = true;
public:
    FrequencyCalculator(unsigned long targetFrequency) : targetFrequency(targetFrequency) {}

    inline bool shouldTick() {
        if (!frequencyLocked) {
            return true;
        }

        auto currFreq = getFrequency();
        return currFreq <= targetFrequency;
    }

    inline bool isFrequencyLocked() const {
        return frequencyLocked;
    }

    inline void setFrequencyLocked(bool frequencyLocked) {
        FrequencyCalculator::frequencyLocked = frequencyLocked;
        this->cycles = 0;
        startTime = std::chrono::high_resolution_clock::now();
    }

    inline void addCycle() {
        ++this->cycles;
        if (this->cycles == 0) {
            // overflow
            // std::cout << "cycle counter overflow, resetting..." << std::endl;
            this->cycles = 0;
            startTime = std::chrono::high_resolution_clock::now();
        }
    }

    inline unsigned long getFrequency() {
        auto now = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(now - startTime).count();
        if (duration >= 1000000) {
            calculatedFrequency = (cycles / duration) * 1000000000;
            // startTime = now;
            // cycles = 0;
        }
        return calculatedFrequency;
    }

    inline long getCycles() const {
        return this->cycles;
    }
};

// nanos / cycles = 250 ns / 1 inst = 250 ns per inst
// 1 inst / 250 ns = 1/(250 * 10^-9) Hz

#endif //NOSFERATU16_FREQUENCYCALCULATOR_H
