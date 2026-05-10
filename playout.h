#ifndef TUNAMCTS_PLAYOUT_H
#define TUNAMCTS_PLAYOUT_H
#include "chess.hpp"
#include "evaluation.h"
#include "random.h"

//random playout with evaluation after depth
inline float playout(const chess::Board &pos) {
    auto moves=chess::Movelist();
    chess::movegen::legalmoves(moves,pos);
    if (moves.empty()) {
        if (pos.inCheck()) {return 0.f;}
        return 0.5f;
    }
    if (pos.isHalfMoveDraw()||pos.isInsufficientMaterial()||pos.isRepetition(1)) {return 0.5f;}
    return eval_prob(evaluation(pos));
}






#endif //TUNAMCTS_PLAYOUT_H
