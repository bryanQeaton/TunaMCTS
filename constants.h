#ifndef TUNAMCTS_CONSTANTS_H
#define TUNAMCTS_CONSTANTS_H

#include "chess.hpp"

//#=======Constants=======#
inline constexpr int BOUND=1000000;
inline constexpr int DRAW_SCORE=0;
inline constexpr int HASH_SIZE=16;
inline constexpr int BUCKET_SIZE=4;
//#=======Helper Functions=======#
constexpr bool is_capture(const chess::Board &pos,const chess::Move &move) {
    if (move==chess::Move()){return false;}
    return pos.at(move.to())!=chess::Piece::NONE;
}
constexpr bool is_promo(const chess::Move &move) {
    if (move==chess::Move()){return false;}

    return move.typeOf()==chess::Move::PROMOTION;
}
constexpr bool is_quiet(const chess::Board &pos,const chess::Move &move) {
    if (move==chess::Move()){return false;}
    return pos.at(move.to())==chess::Piece::NONE&&!is_promo(move);
}
constexpr bool has_non_pawn_material_stm(const chess::Board &pos) {
    auto pieces=pos.us(pos.sideToMove());
    pieces^=pos.pieces(chess::PieceType::PAWN,pos.sideToMove());
    pieces^=pos.pieces(chess::PieceType::KING,pos.sideToMove());
    return !pieces.empty();
}

#endif //TUNAMCTS_CONSTANTS_H
