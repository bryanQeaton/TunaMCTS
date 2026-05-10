#include <chrono>
#include <iostream>


#include "evaluation.h"
#include "chess.hpp"
#include "debug.h"
#include "playout.h"
#include "root.h"
#include "search.h"
#include "uci.h"


int main() {

    //illegal_move_bug(100);


    uci();
    // auto pos=chess::Board(chess::constants::STARTPOS);
    // //auto pos=chess::Board("r2qk2r/pb1nb1pp/1p1pN3/2p5/3pp2P/6PB/PPP2P2/1RBQ1RK1 w kq - 6 16");
    // //auto pos=chess::Board("rn3rk1/p2q1p2/bp2pn1p/3pP3/P2N1Q2/1P3NPP/5PB1/4R1K1 w - - 1 21");
    // auto ret=root(pos,1,0,true);
    // std::cout<<chess::uci::moveToUci(ret.move)<<"\n";
    // std::cout<<ret.value<<"\n";
    // std::cout<<ret.visits<<"\n";



    return 0;
}
