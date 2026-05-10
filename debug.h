#ifndef TUNAMCTS_DEBUG_H
#define TUNAMCTS_DEBUG_H
#include "chess.hpp"
#include "root.h"




//FIXED: caused by draw checks at the start of every node.
inline void illegal_move_bug(const int games) {
    const auto pos=chess::Board(chess::constants::STARTPOS);
    for (int i=0;i<games;i++) {
        auto p=pos;
        while (p.isGameOver().second==chess::GameResult::NONE) {
            auto move=root(p,50).move;
            if (!p.isLegal(move)||move==chess::Move()) {
                throw std::runtime_error("shits fucked");
            }
            p.makeMove(move);

        }
        if (i%1==0){std::cout<<i+1<<"/"<<games<<"\n";}
    }


}




#endif //TUNAMCTS_DEBUG_H
