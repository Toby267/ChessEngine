#pragma once

#include <list>

#include "board/Board.hpp"
#include "board/Move.hpp"

class MoveGenerator {
public:
    //public methods
    static std::list<Move> generateMoves(const Board& board, bool whiteTurn);
};