#ifndef NTUPLECLASSIFIER_TTT_H
#define NTUPLECLASSIFIER_TTT_H
#include <cstdint>
#include <iostream>
#include <vector>
constexpr uint16_t masks[8]={0x7,0x38,0x1c0,0x49,0x92,0x124,0x111,0x54};
struct TTT {
    bool stm=false;
    uint16_t board[2]{};
    uint16_t moves=0x1ff;
    void make_move(const int move) {
        board[stm]|=1<<move;
        moves^=1<<move;
        stm=!stm;
    }
    void undo_move(const int move) {
        stm=!stm;
        moves^=1<<move;
        board[stm]^=1<<move;
    }
    [[nodiscard]] bool is_terminal() const {
        for (const uint16_t &mask:masks) {
            if ((mask&board[!stm])==mask){return true;}
        }
        return false;
    }
    [[nodiscard]] bool is_draw() const {
        if ((board[0]|board[1])==0x1ff){return true;}
        return false;
    }
    std::vector<int> legal_moves() const {
        auto temp=moves;
        std::vector<int> moves;
        while (temp) {
            int move=__builtin_ctz(temp);
            temp&=(temp-1);
            moves.push_back(move);
        }
        return moves;
    }
    friend std::ostream &operator<<(std::ostream &s,const TTT &pos){
        for (int i=0;i<9;i++) {
            if (i%3==0){s<<"\n";}
            if (pos.board[0]&1ull<<i){s<<"X ";}
            else if (pos.board[1]&1ull<<i){s<<"O ";}
            else{s<<"_ ";}
        }
        s<<"\n";
        return s;
    }
};





#endif //NTUPLECLASSIFIER_TTT_H