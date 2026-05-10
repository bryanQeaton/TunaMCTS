#ifndef TUNAMCTS_GAMETREE_H
#define TUNAMCTS_GAMETREE_H
#include <cstdint>
#include <vector>

#include "chess.hpp"
#include "tt.h"


//stores information about the children of a given node.
//the move is stored as well as the index to the node in tree it leads to.
struct Child {
    chess::Move move=chess::Move();
    uint64_t idx=0;
    float prior=0.f;
    Child(const chess::Move &move,const uint64_t idx,const float prior):move(move),idx(idx),prior(prior){};
    Child()=default;
};
//stores information about a node (a game position).
//by storing the moves at the position rather than the position itself we hopefully save space.
//move generation should be deterministic.
struct Node {
    float value=0.f;
    uint64_t visits=0;
    uint64_t hash=0;
    //vector of children for a given node
    std::vector<Child> children;
    //constructors
    Node()=default;
    Node(const float value,const uint64_t visits,const uint64_t hash):value(value),visits(visits),hash(hash){}
    //functions
    float winrate() const {return value/visits;}
    void add_child(const chess::Move &move,const uint64_t idx,const float prior){children.push_back(Child(move,idx,prior));}
    size_t child_count() const {return children.size();}
};
//stores the nodes visited by the mcts search algorithm.
class Game_Tree {
public:
    //constructors
    Game_Tree()=default;
    //functions
    void update_node(const float value,const uint64_t idx) {
        auto &curr=tree[idx];
        curr.value+=value;
        curr.visits++;
        Entry &entry=tt[curr.hash];
        entry.value+=value;
        entry.visits++;
    }
    void add_child_to_node(const chess::Move &move,const int child_idx,const float prior,const uint64_t idx) {
        auto &curr=tree[idx];
        curr.add_child(move,child_idx,prior);
    }
    void clear() {
        tree.clear();
        tree.shrink_to_fit();
    }
    auto &operator[](const uint64_t idx) const {
        //if (idx>=size()){throw std::runtime_error("idx out of bounds!");}
        return tree[idx];
    }
    auto &operator[](const uint64_t idx) {
        //if (idx>=size()){throw std::runtime_error("idx out of bounds!");}
        return tree[idx];
    }
    void add(const float value,const uint64_t visits,const uint64_t hash) {
        Entry &entry=tt[hash];
        if (entry.hash!=hash) {
            entry.hash=hash;
            entry.value=0;
            entry.visits=0;
        }
        tree.push_back(Node(value,visits,hash));
    }
    void pop(){tree.pop_back();}
    auto &back(){return tree.back();}
    size_t size() const {return tree.size();}
    bool empty() const {return tree.empty();}
private:
    //vector of nodes reached by the mcts algorithm
    std::vector<Node> tree{};
};
inline Game_Tree game_tree;



#endif //TUNAMCTS_GAMETREE_H
