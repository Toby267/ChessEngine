#pragma once

#include "board/Move.hpp"
#include "board/Board.hpp"

namespace Zobrist {
    void initZobristValues();
    int generateKey(const Board& board);
}

//might not need all these values for my implementation
typedef struct TranspositionNode {
    //zobrist key
    Move bestMove;
    int depth;
    int score; //could be stored as the heuristic, or keep heuristic constant
    enum NodeType { PV_NODE, ALL_NODE, CUT_NODE } nodeType;
    int age;
} TranspositionNode;

typedef struct pVariation {
    int moveCount{};
    Move moves[15];

    void print() const;
} pVariation;
