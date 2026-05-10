cmake_minimum_required(VERSION 4.2)
project(TunaMCTS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "-O1")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(TunaMCTS main.cpp
#        chess.hpp
        random.h
        ttt.h
        tttmcts.h
        evaluation.h
        search.h
        playout.h
        gametree.h
        constants.h
        uci.h
        root.h
        debug.h
        tt.h

)
