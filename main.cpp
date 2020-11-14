#define SDL_MAIN_HANDLED

#include <iostream>
#include <chrono>

#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>

#include "Config.h"
#include "core/NosferatuEmulator.h"

constexpr int ScreenPosX = 1;
constexpr int ScreenPosY = 19;
constexpr int ScreenWidth = 320;
constexpr int ScreenHeight = 320;

constexpr int MemoryPosX = 325;
constexpr int MemoryPosY = 19;

int focusedMemoryArea = -1;

void PrintString(
        SDL_Renderer *renderer,
        int x,
        int y,
        std::string message,
        int ptsize = 18,
        SDL_Color color = {255, 0, 0}
) {
    TTF_Font *Sans = TTF_OpenFont("font.ttf", 24); //this opens a font style and sets a size

    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Sans, message.c_str(),
                                                       color); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

    SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer,
                                                        surfaceMessage); //now you can convert it into a texture

    SDL_Rect Message_rect; //create a rect
    Message_rect.x = x;  //controls the rect's x coordinate
    Message_rect.y = y; // controls the rect's y coordinte
    Message_rect.w = message.size() * ptsize; // controls the width of the rect
    Message_rect.h = ptsize; // controls the height of the rect

//Now since it's a texture, you have to put RenderCopy in your game loop area, the area where the whole code executes

    SDL_RenderCopy(renderer, Message, NULL,
                   &Message_rect); //you put the renderer's name first, the Message, the crop size(you can ignore this if you don't want to dabble with cropping), and the rect which is the size and coordinate of your texture

//Don't forget to free your surface and texture
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

void DrawAreaBorder(SDL_Renderer *renderer, int x, int y, int w, int h) {
    SDL_Rect rect = {
            x - 1, y - 1,
            w + 2, h + 2
    };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
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

void DrawMemoryMap(SDL_Renderer *renderer, NosferatuEmulator *emu) {
    int k = 0;
    for (int i = 0; i < 256; ++i) {
        for (int j = 0; j < 256; ++j) {
            u16 rgb = emu->getMemory()[k];
            u16 r = rgb & 0xFF;
            u16 g = rgb & 0xFF00;
            u16 b = 0;

            SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);

            SDL_RenderDrawPoint(renderer, MemoryPosX + j * 2, MemoryPosY + i * 2);
            SDL_RenderDrawPoint(renderer, MemoryPosX + j * 2 + 1, MemoryPosY + i * 2);
            SDL_RenderDrawPoint(renderer, MemoryPosX + j * 2, MemoryPosY + i * 2 + 1);
            SDL_RenderDrawPoint(renderer, MemoryPosX + j * 2 + 1, MemoryPosY + i * 2 + 1);

            k++;
        }
    }

    if (focusedMemoryArea >= 0) {
        DrawAreaBorder(renderer, 49, 362, 258, 258);
        SDL_Color colors[] = {
                {255, 0,   0, 0},
                {0,   255, 0, 0}
        };
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
                    12,
                    colors[i % 2]
            );
        }
        for (int q = 373, i = 0; i < 8; ++i, q += 32) {
            PrintString(
                    renderer,
                    22,
                    q,
                    toHex(y + i),
                    12,
                    colors[(1 + i) % 2]
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

void DrawRegisters(SDL_Renderer* renderer, NosferatuEmulator* emu) {
    // todo
}

int main(int argc, char *argv[]) {
    // init emu
    Config config;

    SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow(
            "NOSferatu16",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            841,
            630,
            0
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;
    SDL_Event e;

    NosferatuEmulator emu;

    while (!quit) {
        //Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            //User requests quit
            switch (e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    //do whatever you want to do after a mouse button was pressed,
                    // e.g.:
                    MousePress(renderer, e.button);
                    break;
            }
        }

        try {
            emu.step();
        } catch (const std::invalid_argument &ex) {
            std::cout << std::endl;
            std::cout << ex.what();
            break;
        } catch (const std::exception &ex) {
            std::cout << std::endl << "Unhandled exception? " << ex.what();
            break;
        }

        ClearScreen(renderer);
        DrawMask(renderer);
        // todo draw screen
        DrawMemoryMap(renderer, &emu);
        DrawRegisters(renderer, &emu);
        SDL_RenderPresent(renderer);
    }


    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}