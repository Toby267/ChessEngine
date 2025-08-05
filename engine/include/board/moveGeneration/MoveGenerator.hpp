#pragma once

#include <vector>

#include "board/Board.hpp"

std::vector<Move> generateMoves(Board& board);
bool isKingTargeted(const Board& board);
