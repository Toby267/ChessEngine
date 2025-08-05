#pragma once

#include <vector>

#include "board/Board.hpp"

/**
 * Contains move generation functions
*/

std::vector<Move> generateMoves(Board& board);
bool isKingTargeted(const Board& board);
