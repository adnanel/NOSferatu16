#define SDL_MAIN_HANDLED

#include <iostream>
#include <chrono>

#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>
#include <map>
#include <fstream>

#include "core/NosferatuEmulator.h"
#include "core/FrequencyCalculator.h"
#include "util/SDL_FontCache.h"
#include "io/reader/MnemonicReader.h"
#include "io/converter/ProgramConverter.h"

constexpr int ScreenPosX = 1;
constexpr int ScreenPosY = 19;
constexpr int ScreenWidth = 320;
constexpr int ScreenHeight = 320;

constexpr int MemoryPosX = 325;
constexpr int MemoryPosY = 19;

constexpr int MemoryZoomWidth = 4;
constexpr int MemoryZoomHeight = 16;

constexpr int MemoryZoomHalfWidth = MemoryZoomWidth / 2;
constexpr int MemoryZoomHalfHeight = MemoryZoomHeight / 2;

int focusedMemoryArea = -1;

long long instructions = 0;
constexpr unsigned long targetFrequency = 4 * 1000000; // [Hz]

long targetNanosPerFrame = (1.0 / 15) * 1000000000;
FrequencyCalculator frequencyCalculator(targetFrequency);

FC_Font* fontCache[150] = {0};

bool memoryViewEnabled = true;
bool registerViewEnabled = true;

std::chrono::time_point<std::chrono::high_resolution_clock> lastDrawTime;

enum ExecutionMode {
    Normal,
    Step
};

ExecutionMode executionMode = ExecutionMode::Step;
bool stepPending = false;

inline std::string toString(ExecutionMode mode) {
    switch (mode) {
        case Normal:
            return "Normal";
        case Step:
            return "Step";
    }
    return "??";
}

inline void PrintString(
        SDL_Renderer *renderer,
        int x,
        int y,
        const std::string& message,
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

inline void DrawAreaBorder(SDL_Renderer *renderer, int x, int y, int w, int h, int r = 255, int g = 0, int b = 0) {
    SDL_Rect rect = {
            x - 1, y - 1,
            w + 2, h + 2
    };
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

inline void DrawFilledRect(SDL_Renderer *renderer, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) {
    SDL_Rect rect = {
            x - 1, y - 1,
            w + 2, h + 2
    };
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderFillRect(renderer, &rect);
}


std::string hex2cache[0x1FF] = {};
std::string hex4cache[0x1FFFF] = {};
std::string hex1cache[0x1F] = {};

inline std::string& toHex(int num) {
    char str[32];
    if (!hex2cache[num].empty()) {
        return hex2cache[num];
    }
    sprintf(str, "%02X", num);
    return hex2cache[num] = std::string(str);
}

inline std::string& toHex4(int num) {
    char str[32];
    if (!hex4cache[num].empty()) {
        return hex4cache[num];
    }
    sprintf(str, "%04X", num);
    return hex4cache[num] = std::string(str);
}

inline std::string& toHexSingle(int num) {
    char str[32];
    if (!hex1cache[num].empty()) {
        return hex1cache[num];
    }
    sprintf(str, "%01X", num);
    return hex1cache[num] = std::string(str);
}

void DrawScreen(SDL_Renderer *renderer, NosferatuEmulator *emu) {
    auto vmem = emu->getVideoMemory();
    u16 cword;
    u16 mask = 0;
    u16 wordIndex = 0;
    // word in memory   0x0405
    // word in memory   |0|0|0|0|0|1|0|0|0|0|0|0|0|1|0|1|
    // pixels on screen | | | | | |X| | | | | | | |X| |X|

    // since we have a 320x320 screen, where 1 bit is 1 monochromatic pixel
    // the video memory will be inside the word range 0xC000 - 0xD900
    // (i.e. 6400 words = 6400 * 16 bits = 102400 bits for 320x320 = 102400 pixel)

    DrawAreaBorder(renderer, ScreenPosX, ScreenPosY, ScreenWidth, ScreenHeight);
    PrintString(renderer, 0, 0, "Screen");


    for (int i = 0; i < 320; ++ i) {
        for (int j = 0; j < 320; ++ j) {
            mask = mask >> 1u;
            if (mask == 0) {
                cword = vmem[wordIndex++];
                mask = 0x8000;
            }

            auto val = cword & mask ? 255 : 0;
            SDL_SetRenderDrawColor(renderer, val, val, val, 255);
            SDL_RenderDrawPoint(renderer, ScreenPosX + j, ScreenPosY + i);
        }
    }
}

void DrawMemoryMap(SDL_Renderer *renderer, NosferatuEmulator *emu) {
    int k = 0;
    auto mem = emu->getMemory();

    PrintString(renderer, 325, 0, "Memory map");
    DrawAreaBorder(renderer, MemoryPosX, MemoryPosY, 512, 512);

    for (int i = 0; i < 256; ++i) {
        for (int j = 0; j < 256; ++j) {
            u16 rgb = mem[k];
            u16 r = rgb & 0x00FF;
            u16 g = rgb & 0x0F00;
            u16 b = rgb & 0xF000;

            DrawAreaBorder(renderer, MemoryPosX + j * 2 + 1, MemoryPosY + i * 2 + 1, 0, 0, r, g, b);

            k++;
        }
    }

    if (focusedMemoryArea >= 0) {
        DrawAreaBorder(renderer, 49, 362, 258, 258);
        int x = focusedMemoryArea % 256;
        int y = focusedMemoryArea / 256;

        x -= MemoryZoomHalfWidth;
        y -= MemoryZoomHalfHeight;


        // draw magnifier
        DrawAreaBorder(renderer, MemoryPosX + x * 2, MemoryPosY + y * 2, MemoryZoomWidth * 2, MemoryZoomHeight * 2);

        auto lx = MemoryPosX + x * 2 + MemoryZoomWidth;
        auto ly = MemoryPosY + y * 2 + MemoryZoomHeight * 2;
        SDL_RenderDrawLine(renderer, lx, ly, lx, 580);
        SDL_RenderDrawLine(renderer, lx, 580, 307, 580);


        PrintString(
                renderer,
                22,
                355,
                "HI",
                12
        );
        PrintString(
                renderer,
                45,
                345,
                "LO",
                12
        );
        SDL_RenderDrawLine(renderer, 49, 362, 49 - 16, 362 - 16);

        for (int q = 70, i = 0; i < 4; ++i, q += 64) {
            PrintString(
                    renderer,
                    q,
                    345,
                    toHex(x + i),
                    12
            );
        }
        for (int q = 365, i = 0; i < 16; ++i, q += 16) {
            PrintString(
                    renderer,
                    22,
                    q,
                    toHex(y + i),
                    12
            );
        }

        auto mem = emu->getMemory();
        int w = 64;
        int h = 16;

        for (int i = 0; i < 16; ++i) {
            for (int j = 0; j < 4; ++j) {
                auto rgb = mem[(y + i) * 256 + (x + j)];

                PrintString(renderer, 50 + j * w + 5, 363 + i * h + 1, toHex4(rgb), 14);

                // DrawFilledRect(renderer, 50 + j * w + 1, 363 + i * h + 1, w - 1, h - 1, r, g, b);
                DrawAreaBorder(renderer, 50 + j * w, 363 + i * h, w, h);
            }
        }
    }
}

void ClearScreen(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}

void KeyPress(SDL_Renderer *pRenderer, SDL_KeyboardEvent event, NosferatuEmulator* emu) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_F7:
            switch (executionMode) {
                case Normal:
                    executionMode = ExecutionMode::Step;
                    stepPending = false;
                    break;
                case Step:
                    executionMode = ExecutionMode::Normal;
                    break;
            }
            frequencyCalculator.reset();
            break;
        case SDL_SCANCODE_F8:
            stepPending = true;
            break;
        case SDL_SCANCODE_F11:
            memoryViewEnabled = !memoryViewEnabled;
            break;
        case SDL_SCANCODE_F10:
            registerViewEnabled = !registerViewEnabled;
            break;
    }

    emu->KeyPress(event.keysym.scancode);
}

void KeyRelease(SDL_Renderer *pRenderer, SDL_KeyboardEvent event, NosferatuEmulator* emu) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_F12:
            frequencyCalculator.setFrequencyLocked(!frequencyCalculator.isFrequencyLocked());
            break;
    }

    emu->KeyRelease(event.keysym.scancode);
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
            x = std::min(std::max(MemoryZoomHalfWidth, x), 255 - MemoryZoomHalfWidth + 1);
            y = std::min(std::max(MemoryZoomHalfHeight, y), 255 - MemoryZoomHalfHeight + 1);

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

    PrintString(renderer, RegisterAreaX + 20, 285 - 16, "Next instruction", 14);
    DrawAreaBorder(renderer, RegisterAreaX + 20, 285, 415 - 40, 30);

    auto nextInstruction = emu->getMemoryValueUnsigned(emu->getRegUnsigned(0xFF));
    constexpr int buffSize = 32;
    char buff[buffSize];
    int i = buffSize - 1;
    buff[i--] = 0;
    int cnt = 0;
    while (nextInstruction) {
        if (cnt == 4 || cnt == 8 || cnt == 12) {
            buff[i--] = ' ';
        }
        buff[i--] = '0' + (nextInstruction % 2);
        ++cnt;
        nextInstruction >>= 1;
    }
    while (cnt < 16) {
        buff[i--] = '0';
        cnt++;
    }
    PrintString(renderer, RegisterAreaX + 40, 285,buff + i + 1, 31);
}

inline void adjustFreq(unsigned long& freq, std::string& unit) {
    if (freq >= 1000) {
        freq /= 1000;
        unit = " [kHz]";
    } else {
        unit = " [Hz]";
    }
}

void DrawPerformance(SDL_Renderer *renderer, NosferatuEmulator *emu) {
    PrintString(renderer, 850, 330, "Performance");
    DrawAreaBorder(renderer, 850, 350, 415, 180);

    std::string unit;
    std::string realUnit;
    auto freq = targetFrequency;
    auto realFreq = frequencyCalculator.getFrequency();

    adjustFreq(freq, unit);
    adjustFreq(realFreq, realUnit);

    PrintString(renderer, 855, 360, "Cached instructions: " + std::to_string(emu->getCachedInstructionCount()), 13);
    PrintString(renderer, 855, 375, "Target frequency: " + std::to_string(freq) + unit, 13);
    PrintString(renderer, 855, 390, "Real frequency: " + std::to_string(realFreq) + realUnit, 13);



    PrintString(renderer, 855, 450, std::string("[F7]: Toggle execution mode (current mode: ") + toString(executionMode) + ")", 13);
    if (executionMode == ExecutionMode::Step) {
        PrintString(renderer, 855, 465, "[F8]: Perform step", 13);
    }

    PrintString(renderer, 855, 480, "[F10]: Toggle register view", 13);
    PrintString(renderer, 855, 495, "[F11]: Toggle memory view", 13);
    PrintString(renderer, 855, 510, "[F12]: Toggle frequency limiter", 13);
}

int main(int argc, char *argv[]) {
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

    auto txtProgram = std::ifstream("programs/noise.txt");
    ProgramReader* reader = new MnemonicReader(txtProgram);


    emu.loadProgram(reader);

    while (!quit) {
        auto now = std::chrono::high_resolution_clock::now();

        { // the following should be executed with the target frequency
            if (
                    (frequencyCalculator.shouldTick() && executionMode == ExecutionMode::Normal) ||
                    (executionMode == ExecutionMode::Step && stepPending)
            ) {
                emu.step();
                ++instructions;
                frequencyCalculator.addCycle();
                stepPending = false;
            }
        }

        // everything below has to be executed on VSync, but should not block the main thread to compensate
        // for the performance.
        {
            auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>(now - lastDrawTime).count();
            if (delta >= targetNanosPerFrame) {
                // std::cout<<delta<< " nanos have passed, refreshing screen " << targetMicrosPerFrame << std::endl;
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
                        case SDL_KEYUP:
                            KeyRelease(renderer, e.key, &emu);
                            break;
                        case SDL_KEYDOWN:
                            KeyPress(renderer, e.key, &emu);
                            break;
                    }
                }

                ClearScreen(renderer);
                DrawScreen(renderer, &emu);
                if (memoryViewEnabled) {
                    DrawMemoryMap(renderer, &emu);
                }
                if (registerViewEnabled) {
                    DrawRegisters(renderer, &emu);
                }
                DrawPerformance(renderer, &emu);
                SDL_RenderPresent(renderer);
            }
        }
    }


    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
