#ifndef TUNAMCTS_GAMETREE_H
#define TUNAMCTS_GAMETREE_H
#include <cstdint>
#include <vector>

#include "chess.hpp"

//stores the nodes visited by the mcts search algorithm.
class Game_Tree {
public:
    //stores information about a node (a game position).
    //by storing the moves at the position rather than the position itself we hopefully save space.
    //move generation should be deterministic.
    struct Node {
        float value=0.f;
        uint64_t visits=0;
        uint64_t hash=0;
        //stores information about the children of a given node.
        //the move is stored as well as the index to the node in tree it leads to.
        struct Child {
            chess::Move move=chess::Move();
            int idx=0;
            float prior=0.f;
            Child(const chess::Move &move,const int idx,const float prior):move(move),idx(idx),prior(prior){};
            Child()=default;
        };
        //vector of children for a given node
        std::vector<Child> children;
        //constructors
        Node()=default;
        Node(const float value,const uint64_t visits,const uint64_t hash):value(value),visits(visits),hash(hash){}
        //functions
        float winrate() const {return value/visits;}
        void add_child(const chess::Move &move,const int idx,const float prior){children.push_back(Child(move,idx,prior));}
        size_t child_count() const {return children.size();}

    };
    //constructors
    Game_Tree()=default;
    //functions
    void update_node(const float value,const int idx) {
        auto &curr=tree[idx];
        curr.value+=value;
        curr.visits++;
    }
    void add_child_to_node(const chess::Move &move,const int child_idx,const float prior,const int idx) {
        auto &curr=tree[idx];
        curr.add_child(move,child_idx,prior);
    }
    void clear(){tree.clear();}
    auto &operator[](const int idx) const {return tree[idx];}
    auto &operator[](const int idx) {return tree[idx];}
    void add(const float value,const uint64_t visits,const uint64_t hash){tree.push_back(Node(value,visits,hash));}
    void add(const Node &node){tree.push_back(node);}
    void add(){tree.push_back(Node());}
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
