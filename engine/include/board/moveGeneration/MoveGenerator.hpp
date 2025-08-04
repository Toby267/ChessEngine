#pragma once

#include <vector>

#include "board/Board.hpp"
#include "board/BoardUtil.hpp"

std::vector<Move> generateMoves(Board& board, WhiteTurn whiteTurn);
bool isKingTargeted(Board& board, WhiteTurn whiteTurn);
