#pragma once

#include <vector>

#include "board/Board.hpp"
#include "board/Move.hpp"

// class MoveGenerator {
// public:
//     //public methods
//     static std::list<Move> generateMoves(const Board& board, bool whiteTurn);
// };

std::vector<Move> generateMoves(const Board& board, bool whiteTurn);

void bitScan();
