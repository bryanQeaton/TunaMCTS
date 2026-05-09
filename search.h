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
    }
}
inline std::vector<float> policy(const chess::Movelist &moves) {
    std::vector<float> po(moves.size(),0.f);
    float sum=0;
    for (auto move:moves){sum+=move.score();}
    for (int i=0;i<po.size();i++){po[i]=moves[i].score()/sum;}
    return po;
}

inline Game_Tree::Node::Child uct_best(const float C,chess::Board &pos,const chess::Movelist &moves,const Game_Tree::Node &node) {
    auto best_child=node.children[0];
    float best_uct=-INFINITY;

    for (const auto &child:node.children) {
        const auto &child_node=game_tree[child.idx];
        const float wr=child_node.visits? 1.f-child_node.winrate(): 0.5f;
        float uct=wr+C*child.prior*sqrtf(node.visits)/(1.f+child_node.visits);
        if (uct>best_uct) {
            best_uct=uct;
            best_child=child;
        }
    }
    return best_child;
}
inline float search(const float puct,chess::Board &pos,const int playout_maxdepth=100,const int idx=0) {
    auto moves=chess::Movelist();
    chess::movegen::legalmoves(moves,pos);
    if (moves.empty()) {return pos.inCheck()? 0.0f: 0.5f;}
    if (pos.isInsufficientMaterial()||pos.isHalfMoveDraw()||pos.isRepetition(1)) {return 0.5f;}
    if (game_tree.empty()) {//if tree is empty
        const float value=playout(pos,playout_maxdepth);
        game_tree.add(value,1,pos.hash());
        return value;
    }
    assert(!game_tree.empty()&&"Game tree is empty!");
    assert(game_tree[idx].hash==pos.hash()&&"Position hash doesn't match tree stored hash!");
    // if (game_tree[idx].child_count()!=moves.size()) {
    //     //expand
    //     const auto child_move=moves[game_tree[idx].child_count()];
    //     game_tree.add_child_to_node(child_move,game_tree.size(),0,idx);
    //     //playout
    //     pos.makeMove(child_move);
    //     float value=playout(pos,playout_maxdepth);
    //     uint64_t child_hash=pos.hash();
    //     pos.unmakeMove(child_move);
    //     game_tree.add(value,1,child_hash);
    //     value=1.f-value;
    //     game_tree.update_node(value,idx);
    //     return value;
    // }
    if (game_tree[idx].child_count()==0) {
        score_moves(pos,moves);
        const auto pol=policy(moves);
        int best_idx=0;
        float best_policy=-INFINITY;
        for (int m=0;m<moves.size();m++) {
            const int child_idx=game_tree.size();
            game_tree.add_child_to_node(
                moves[m],
                child_idx,
                pol[m],
                idx
            );
            game_tree.add(
                0,
                0,
                pos.zobristAfter(moves[m])
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
