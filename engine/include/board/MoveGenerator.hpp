#pragma once

#include <vector>

#include "board/Board.hpp"
#include "board/Move.hpp"

// class MoveGenerator {
// public:
//     //public methods
//     static std::list<Move> generateMoves(const Board& board, bool whiteTurn);
// };

//TODO: remember to make const again
std::vector<Move> generateMoves(Board& board, bool whiteTurn);

void bitScan();
