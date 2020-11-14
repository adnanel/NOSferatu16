#define SDL_MAIN_HANDLED

#include <iostream>
#include <chrono>

#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>
#include <map>

#include "Config.h"
#include "core/NosferatuEmulator.h"
#include "core/FrequencyCalculator.h"
#include "util/SDL_FontCache.h"

constexpr int ScreenPosX = 1;
constexpr int ScreenPosY = 19;
constexpr int ScreenWidth = 320;
constexpr int ScreenHeight = 320;

constexpr int MemoryPosX = 325;
constexpr int MemoryPosY = 19;

int focusedMemoryArea = -1;

long long instructions = 0;
constexpr long targetFrequency = 4 * 1000000; // [Hz]

double targetMicrosPerFrame = (1.0 / 15) * 1000000;
double targetMicrosPerEmuTick = 0; // todo

FrequencyCalculator frequencyCalculator;

FC_Font* fontCache[150] = {0};

std::chrono::time_point<std::chrono::high_resolution_clock> lastDrawTime;
std::chrono::time_point<std::chrono::high_resolution_clock> lastEmuTickTime;

void PrintString(
        SDL_Renderer *renderer,
        int x,
        int y,
        std::string message,
        int ptsize = 18,
        SDL_Color color = {255, 0, 0, 255}
) {
    FC_Font *Sans;
    if (fontCache[ptsize]) {
        Sans = fontCache[ptsize];
    } else {
        Sans = FC_CreateFont();
        FC_LoadFont(Sans, renderer, "font.TTF", ptsize, color, TTF_STYLE_NORMAL);
        fontCache[ptsize] = Sans;
    }

    FC_Draw(Sans, renderer, x, y, message.c_str());
}

void DrawAreaBorder(SDL_Renderer *renderer, int x, int y, int w, int h, int r = 255, int g = 0, int b = 0) {
    SDL_Rect rect = {
            x - 1, y - 1,
            w + 2, h + 2
    };
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

void DrawFilledRect(SDL_Renderer *renderer, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) {
    SDL_Rect rect = {
            x - 1, y - 1,
            w + 2, h + 2
    };
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void DrawMask(SDL_Renderer *renderer) {
    DrawAreaBorder(renderer, ScreenPosX, ScreenPosY, ScreenWidth, ScreenHeight);
    DrawAreaBorder(renderer, MemoryPosX, MemoryPosY, 512, 512);

    PrintString(renderer, 0, 0, "Screen");
    PrintString(renderer, 325, 0, "Memory map");
}

std::string toHex(int num) {
    char str[32];
    sprintf(str, "%02X", num);
    return std::string(str);
}

std::string toHex4(int num) {
    char str[32];
    sprintf(str, "%04X", num);
    return std::string(str);
}

std::string toHexSingle(int num) {
    char str[32];
    sprintf(str, "%01X", num);
    return std::string(str);
}

void DrawMemoryMap(SDL_Renderer *renderer, NosferatuEmulator *emu) {
    int k = 0;
    for (int i = 0; i < 256; ++i) {
        for (int j = 0; j < 256; ++j) {
            u16 rgb = emu->getMemory()[k];
            u16 r = rgb & 0xFF;
            u16 g = rgb & 0xFF00;
            u16 b = 0;

            DrawAreaBorder(renderer, MemoryPosX + j * 2 + 1, MemoryPosY + i * 2 + 1, 0, 0, r, g, b);

            k++;
        }
    }

    if (focusedMemoryArea >= 0) {
        DrawAreaBorder(renderer, 49, 362, 258, 258);
        int x = focusedMemoryArea % 256;
        int y = focusedMemoryArea / 256;

        x -= 3;
        y -= 3;

        for (int q = 53, i = 0; i < 8; ++i, q += 32) {
            PrintString(
                    renderer,
                    q,
                    345,
                    toHex(x + i),
                    12
            );
        }
        for (int q = 373, i = 0; i < 8; ++i, q += 32) {
            PrintString(
                    renderer,
                    22,
                    q,
                    toHex(y + i),
                    12
            );
        }

        auto mem = emu->getMemory();
        int w = 32;
        int h = 32;

        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                auto rgb = mem[(y + i) * 256 + (x + j)];

                u16 r = rgb & 0xFF;
                u16 g = rgb & 0xFF00;
                u16 b = 0;


                DrawFilledRect(renderer, 50 + j * w + 1, 363 + i * h + 1, w - 1, h - 1, r, g, b);
                DrawAreaBorder(renderer, 50 + j * w, 363 + i * h, w, h);
            }
        }
    }
}

void ClearScreen(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}

void MousePress(SDL_Renderer *renderer, SDL_MouseButtonEvent &b) {
    if (b.button == SDL_BUTTON_LEFT) {
        //handle a left-click
        if (
                b.x >= MemoryPosX &&
                b.x <= MemoryPosX + 512 &&
                b.y >= MemoryPosY &&
                b.y <= MemoryPosY + 512
                ) {
            auto x = (b.x - MemoryPosX) / 2;
            auto y = (b.y - MemoryPosY) / 2;
            x = std::min(std::max(3, x), 251);
            y = std::min(std::max(3, y), 251);

            focusedMemoryArea = x + (y * 256);
        } else {
            focusedMemoryArea = -1;
        }
    }
}

void DrawRegisters(SDL_Renderer *renderer, NosferatuEmulator *emu) {
    int w = 104;
    int h = 32;

    constexpr int RegisterAreaX = 850;

    PrintString(renderer, RegisterAreaX, 2, "Registers");
    DrawAreaBorder(renderer, RegisterAreaX, 19, 415, 300);


    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            DrawAreaBorder(
                    renderer,
                    RegisterAreaX + j * w + 5,
                    25 + i * h * 2,
                    w - 10,
                    h
            );
            PrintString(renderer, RegisterAreaX + j * w + 15, 60 + (i) * h * 2, toHexSingle(i * 4 + j), 18);
            PrintString(renderer, RegisterAreaX + j * w + 15, 35 + (i) * h * 2, toHex4(emu->getRegUnsigned(i * 4 + j)), 14);
        }
    }
}

inline void adjustFreq(long& freq, std::string& unit) {
    if (freq >= 1000000) {
        freq /= 1000000;
        unit = " [MHz]";
    } else if (freq >= 1000) {
        freq /= 1000;
        unit = " [kHz]";
    } else {
        unit = " [Hz]";
    }
}

void DrawPerformance(SDL_Renderer *renderer, NosferatuEmulator *emu) {
    PrintString(renderer, 850, 330, "Performance");
    DrawAreaBorder(renderer, 850, 350, 415, 180);

    PrintString(renderer, 855, 360, "Instructions per second: ", 13);

    std::string unit;
    std::string realUnit;
    auto freq = targetFrequency;
    auto realFreq = frequencyCalculator.getFrequency();

    adjustFreq(freq, unit);
    adjustFreq(realFreq, realUnit);

    PrintString(renderer, 855, 375, "Target frequency: " + std::to_string(freq) + unit, 13);
    PrintString(renderer, 855, 390, "Real frequency: " + std::to_string(realFreq) + realUnit, 13);
}

int main(int argc, char *argv[]) {
    // init emu
    Config config;

    lastDrawTime = std::chrono::high_resolution_clock::now();

    SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow(
            "NOSferatu16",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            1280,
            630,
            0
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;
    SDL_Event e;

    NosferatuEmulator emu;

    while (!quit) {
        auto now = std::chrono::high_resolution_clock::now();

        { // the following should be executed with the target frequency
            auto delta = std::chrono::duration_cast<std::chrono::microseconds>(now - lastEmuTickTime).count();
            if (delta >= targetMicrosPerEmuTick) {
                lastEmuTickTime = now;


                auto cycles = emu.step();
                ++instructions;


                frequencyCalculator.addCycles(cycles);
                frequencyCalculator.step();
            }
        }

        // everything below has to be executed on VSync, but should not block the main thread to compensate
        // for the performance.
        {
            auto delta = std::chrono::duration_cast<std::chrono::microseconds>(now - lastDrawTime).count();
            if (delta >= targetMicrosPerFrame) {
                lastDrawTime = now;

                while (SDL_PollEvent(&e) != 0) {
                    //User requests quit
                    switch (e.type) {
                        case SDL_QUIT:
                            quit = true;
                            break;
                        case SDL_MOUSEBUTTONDOWN:
                            MousePress(renderer, e.button);
                            break;
                    }
                }

                ClearScreen(renderer);
                DrawMask(renderer);
                // todo draw screen
                DrawMemoryMap(renderer, &emu);
                DrawRegisters(renderer, &emu);
                DrawPerformance(renderer, &emu);
                SDL_RenderPresent(renderer);
            }
        }
    }


    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}