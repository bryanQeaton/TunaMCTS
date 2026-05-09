#ifndef TUNAMCTS_PLAYOUT_H
#define TUNAMCTS_PLAYOUT_H
#include "chess.hpp"
#include "evaluation.h"
#include "random.h"

//random playout with evaluation after depth
inline float _playout(chess::Board &pos,const int depth=100) {
    if (depth<=0){return eval_prob(evaluation(pos));}
    auto moves=chess::Movelist();
    chess::movegen::legalmoves(moves,pos);
    if (moves.empty()) {
        if (pos.inCheck()) {return 0.f;}
        return 0.5f;
    }
    if (pos.isHalfMoveDraw()||pos.isInsufficientMaterial()||pos.isRepetition(1)) {return 0.5f;}
    const chess::Move best=moves[fast_rand(moves.size())];
    pos.makeMove(best);
    return 1.f-_playout(pos,depth-1);
}
//playout wrapper function, call this.
inline float playout(chess::Board pos,const int depth=100) {
    auto moves=chess::Movelist();
    chess::movegen::legalmoves(moves,pos);
    if (moves.empty()) {return pos.inCheck()? 0.0f: 0.5f;}
    if (pos.isInsufficientMaterial()||pos.isHalfMoveDraw()||pos.isRepetition(1)) {return 0.5f;}
    return _playout(pos,depth);
}






#endif //TUNAMCTS_PLAYOUT_H
