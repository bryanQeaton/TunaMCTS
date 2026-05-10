#ifndef TUNAMCTS_SEARCH_H
#define TUNAMCTS_SEARCH_H
#include "chess.hpp"
#include "constants.h"
#include "gametree.h"
#include "playout.h"

inline void score_moves(const chess::Board &pos,chess::Movelist &moves) {
    for (auto &move:moves) {
        move.setScore(move.score()+10000*(pos.givesCheck(move)!=chess::CheckType::NO_CHECK));
        if (is_capture(pos,move)) {
            move.setScore(move.score()+mat[pos.at(move.from()).type()]-.5*mat[pos.at(move.to()).type()]);
        }
        if (is_promo(move)) {
            move.setScore(move.score()+10000);
        }
        move.setScore(move.score()+10000);
        //psqt
         if (pos.sideToMove()==chess::Color::WHITE) {
             move.setScore(move.score()+mg_tables[static_cast<int>(pos.at(move.from()).type().internal())][flip(move.to().index())]-mg_tables[static_cast<int>(pos.at(move.from()).type().internal())][flip(move.from().index())]);
         }
         else {
             move.setScore(move.score()+mg_tables[static_cast<int>(pos.at(move.from()).type().internal())][move.to().index()]-mg_tables[static_cast<int>(pos.at(move.from()).type().internal())][move.from().index()]);
         }
    }
}
inline std::vector<float> policy(const chess::Movelist &moves) {
    std::vector po(moves.size(),0.f);
    float sum=0;
    for (auto move:moves){sum+=move.score();}
    for (int i=0;i<po.size();i++){po[i]=moves[i].score()/sum;}
    return po;
}

inline Child uct_best(const float C,chess::Board &pos,const chess::Movelist &moves,const Node &node) {
    auto best_child=node.children[0];
    float best_uct=-INFINITY;
    for (const auto &child:node.children) {
        const auto &child_node=game_tree[child.idx];
        float wr=child_node.visits? 1.f-child_node.winrate(): 0.5f;
        const float uct=wr+C*child.prior*sqrtf(node.visits)/(1.f+child_node.visits);
        if (uct>best_uct) {
            best_uct=uct;
            best_child=child;
        }
    }
    return best_child;
}
inline float search(const float puct,chess::Board &pos,const int playout_maxdepth=100,const uint64_t idx=0) {
    if (game_tree.empty()) {//if tree is empty
        const float value=playout(pos,playout_maxdepth);
        game_tree.add(value,1,pos.hash());
        return value;
    }
    auto moves=chess::Movelist();
    chess::movegen::legalmoves(moves,pos);
    if (moves.empty()) {
        const float value=pos.inCheck()? 0.0f: 0.5f;
        game_tree.update_node(value,idx);
        return value;
    }
    if (idx!=0&&(pos.isInsufficientMaterial()||pos.isHalfMoveDraw()||pos.isRepetition(1))) {
        game_tree.update_node(0.5f,idx);
        return 0.5f;
    }
    // if (idx>=game_tree.size()) {throw std::runtime_error("search(): node index out of bounds");}
    // if (game_tree[idx].hash!=pos.hash()){throw std::runtime_error("Hash difference!");}
    // if (game_tree.empty()){throw std::runtime_error("Empty Tree!");}
    if (game_tree[idx].child_count()==0) {
        score_moves(pos,moves);
        const auto pol=policy(moves);
        uint64_t best_idx=0;
        float best_policy=-INFINITY;
        for (int m=0;m<moves.size();m++) {
            const uint64_t child_idx=game_tree.size();
            game_tree.add(
                0.5f,
                0,
                pos.zobristAfter(moves[m])
            );
            game_tree.add_child_to_node(
                moves[m],
                child_idx,
                pol[m],
                idx
            );
            if (pol[m]>best_policy) {
                best_policy=pol[m];
                best_idx=m;
            }
        }
        const auto best_child=game_tree[idx].children[best_idx];
        pos.makeMove(best_child.move);
        float value=playout(pos,playout_maxdepth);
        pos.unmakeMove(best_child.move);
        game_tree.update_node(value,best_child.idx);
        value=1.f-value;
        game_tree.update_node(value,idx);
        return value;
    }
    //select
    const auto best=uct_best(puct,pos,moves,game_tree[idx]);
    pos.makeMove(best.move);
    const float value=1.f-search(puct,pos,playout_maxdepth,best.idx);
    pos.unmakeMove(best.move);
    game_tree.update_node(value,idx);
    return value;
}





#endif //TUNAMCTS_SEARCH_H
