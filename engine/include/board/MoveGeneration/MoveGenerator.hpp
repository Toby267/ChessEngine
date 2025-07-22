#pragma once

#include <vector>

#include "board/Board.hpp"
#include "board/BoardUtil.hpp"

//TODO: remember to make const again
std::vector<Move> generateMoves(Board& board, bool whiteTurn, SquareIndex temp);
