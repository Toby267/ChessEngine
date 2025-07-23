#pragma once

#include <vector>

#include "board/Board.hpp"

//TODO: remember to make const again
std::vector<Move> generateMoves(Board& board, bool whiteTurn);
