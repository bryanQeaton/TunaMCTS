#ifndef TUNAMCTS_PLAYOUT_H
#define TUNAMCTS_PLAYOUT_H
#include "chess.hpp"
#include "evaluation.h"
#include "random.h"

//random playout with evaluation after depth
inline float _playout(chess::Board &pos,const int depth=100) {
    auto moves=chess::Movelist();
    chess::movegen::legalmoves(moves,pos);
    if (moves.empty()) {
        if (pos.inCheck()) {return 0.f;}
        return 0.5f;
    }
    if (pos.isHalfMoveDraw()||pos.isInsufficientMaterial()||pos.isRepetition(1)) {return 0.5f;}
    if (depth<=0){return eval_prob(evaluation(pos));}
    const chess::Move best=moves[fast_rand(moves.size())];
    pos.makeMove(best);
    return 1.f-_playout(pos,depth-1);
}
//playout wrapper function, call this.
inline float playout(chess::Board pos,const int depth=100) {return _playout(pos,depth);}






#endif //TUNAMCTS_PLAYOUT_H
