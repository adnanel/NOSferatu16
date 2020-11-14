cmake_minimum_required(VERSION 3.17)
project(NOSferatu16)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS -static-libstdc++)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(NOSferatu16 main.cpp io/BinaryReader.cpp io/BinaryReader.h Config.cpp Config.h core/Instruction.cpp core/Instruction.h types.h core/NosferatuEmulator.cpp core/NosferatuEmulator.h core/Operations.cpp core/Operations.h)

target_link_libraries(NOSferatu16 SDL2 SDL2_ttf)
