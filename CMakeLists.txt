cmake_minimum_required(VERSION 3.17)
project(NOSferatu16)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS -static-libstdc++)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
# set(CMAKE_CXX_FLAGS_DEBUG "-O3")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(NOSferatu16 main.cpp util/SDL_FontCache.c util/SDL_FontCache.h io/reader/BinaryReader.cpp io/reader/BinaryReader.h core/Instruction.cpp core/Instruction.h types.h core/NosferatuEmulator.cpp core/NosferatuEmulator.h core/Operations.cpp core/Operations.h core/FrequencyCalculator.cpp core/FrequencyCalculator.h io/reader/ProgramReader.cpp io/reader/ProgramReader.h io/reader/MnemonicReader.cpp io/reader/MnemonicReader.h io/converter/ProgramConverter.cpp io/converter/ProgramConverter.h)

target_link_libraries(NOSferatu16 SDL2 SDL2_ttf)
