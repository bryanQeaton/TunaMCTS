#ifndef TUNAMCTS_DEBUG_H
#define TUNAMCTS_DEBUG_H
#include "chess.hpp"
#include "root.h"

inline void illegal_move_bug(const int games) {
    auto pos=chess::Board(chess::constants::STARTPOS);
    for (int i=0;i<games;i++) {
        auto p=pos;
        while (p.isGameOver().second==chess::GameResult::NONE) {
            p.makeMove(root(p,50).move);
        }
        if (i%1==0){std::cout<<i+1<<"/"<<games<<"\n";}
    }


}




#endif //TUNAMCTS_DEBUG_H
