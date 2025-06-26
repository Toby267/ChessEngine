#pragma once

#include <vector>

#include "board/Board.hpp"
#include "board/Move.hpp"
#include "board/BoardEnums.hpp"

enum Direction {
    NORTH,
    EAST,
    SOUTH,
    WEST,
    NORTH_EAST,
    NORTH_WEST,
    SOUTH_EAST,
    SOUTH_WEST
};

//holy hell this is cursed
uint64_t (*const rayFunctions[])(SquareIndex) = {calcNorthMask, calcEastMask, calcSouthMask, calcWestMask, calcNorthEastMask, calcNorthWestMask, calcSouthEastMask, calcSouthWestMask};

//TODO: remember to make const again
std::vector<Move> generateMoves(Board& board, bool whiteTurn, SquareIndex temp);
