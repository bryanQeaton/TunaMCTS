#ifndef TUNAMCTS_TTTMCTS_H
#define TUNAMCTS_TTTMCTS_H
#include <assert.h>

#include "ttt.h"
#include "random.h"


//stores the nodes visited by the mcts search algorithm.
class Game_Tree {
public:
    //stores information about a node (a game position).
    //by storing the moves at the position rather than the position itself we hopefully save space.
    //move generation should be deterministic.
    struct Node {
        float value=0.f;
        uint64_t visits=0;
        //stores information about the children of a given node.
        //the move is stored as well as the index to the node in tree it leads to.
        struct Child {
            int move{};
            int idx{};
            Child(const int move,const int idx):move(move),idx(idx){};
        };
        //vector of children for a given node
        std::vector<Child> children;
        //constructors
        Node()=default;
        Node(const float value,const uint64_t visits):value(value),visits(visits){}
        //functions
        void add_child(const int move,const int idx){children.push_back(Child(move,idx));}
        size_t child_count() const {return children.size();}
    };
    //constructors
    Game_Tree()=default;
    //functions
    void update_node(const int value,const int idx) {
        auto &curr=tree[idx];
        curr.value+=value;
        curr.visits++;
    }
    void add_child_to_node(const int move,const int child_idx,const int idx) {
        auto &curr=tree[idx];
        curr.add_child(move,child_idx);
    }
    auto &operator[](const int idx) const {return tree[idx];}
    auto &operator[](const int idx) {return tree[idx];}
    void add(const float value){tree.push_back(Node(value,1));}
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

//returns a random playout from the side to move between 0,1. use a copy of the position to initialize.
inline float playout(TTT &pos) {
    if (pos.is_terminal()){return 0.f;}
    if (pos.is_draw()){return 0.5f;}
    auto moves=pos.moves;
    int size=0;
    int movelist[9]{};
    while (moves) {
        const int idx=__builtin_ctz(moves);
        moves&=(moves-1);
        movelist[size]=idx;
        size++;
    }
    pos.make_move(movelist[fast_rand(size)]);
    return 1.f-playout(pos);
}

inline float search(TTT &pos,Game_Tree &game_tree,const int idx=0) {
    if (pos.is_terminal()) {return 0.f;}
    if (pos.is_draw()) {return 0.5f;}
    if (game_tree.empty()) { //if the game_tree is empty
        auto p=pos;
        const float value=playout(p);
        game_tree.add(value);
        return value;
    } //ensure atleast one node is in game_tree
    const auto &node=game_tree[idx];
    assert(!game_tree.empty());
    const auto moves=pos.legal_moves();
    if (moves.size()!=node.child_count()) {
        //2.expansion
        const auto child_idx=game_tree.size();
        const int child_move=moves[game_tree[idx].child_count()];
        game_tree.add_child_to_node(child_move,child_idx,idx);
        //3.playout
        pos.make_move(child_move);
        auto p=pos;
        pos.undo_move(child_move);
        float value=playout(p);
        game_tree.add(value);
        value=1.f-value;
        game_tree.update_node(value,idx);
        return value;
    }
    //1.selection
    assert(moves.size()==node.children.size());
    assert(node.visits>0);
    auto best_child=node.children[0];
    float best_uct=-INFINITY;
    for (const auto &child:node.children) {
        const auto &child_node=game_tree[child.idx];
        const float wr=1.f-(child_node.value/child_node.visits);
        const float uct=wr+1.1*sqrtf(logf(node.visits)/child_node.visits);
        if (uct>best_uct) {
            best_uct=uct;
            best_child=child;
        }
    }
    pos.make_move(best_child.move);
    const float value=1.f-search(pos,game_tree,best_child.idx);
    pos.undo_move(best_child.move);
    game_tree.update_node(value,idx);
    return value;
}



#endif //TUNAMCTS_TTTMCTS_H
