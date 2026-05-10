#ifndef TUNAMCTS_ROOT_H
#define TUNAMCTS_ROOT_H
#include "chess.hpp"
#include "search.h"
#include "gametree.h"
#include "tt.h"

struct Engine_return {
    float value{}; //value of winning move
    uint64_t visits{};
    chess::Move move{};
};

inline Engine_return root(chess::Board &pos,const int time_limit,const int sample_limit=0,const bool verbose=false) {
    game_tree.clear(); //temporary solution, aim to prune the irrelevant parts of the tree instead
    tt.clear(); // this might not be necessary
    auto moves=chess::Movelist();
    chess::movegen::legalmoves(moves,pos);
    if (moves.size()==1){return {.5f,0,moves[0]};}
    uint64_t counter=0;
    constexpr float puct=1.8;
    constexpr int depth=0;
    const auto t0=std::chrono::high_resolution_clock::now();
    size_t time=0;
    while (counter<sample_limit||sample_limit==0) {
        search(puct,pos,depth,0);
        const auto t1=std::chrono::high_resolution_clock::now();
        time=std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count();
        if (time>=time_limit&&counter>0){break;}
        counter++;
    }
    if (verbose){std::cout<<"time: "<<time<<"\n========\n";}
    const auto node=game_tree[0];
    auto best_child=node.children[0];
    float best_value=-INFINITY;
    for (auto child:node.children) {
        if (verbose){std::cout<<chess::uci::moveToUci(child.move)<<" "<<1.f-game_tree[child.idx].winrate()<<"\n";}
        auto value=1.f-game_tree[child.idx].winrate();
        if (value>best_value) {
            best_value=value;
            best_child=child;
        }
    }
    return {1.f-game_tree[best_child.idx].winrate(),counter,best_child.move};
}



#endif //TUNAMCTS_ROOT_H
