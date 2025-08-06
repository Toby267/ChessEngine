#pragma once

#include "board/Board.hpp"

/**
 * Defines functions for a fast pesto evaluation of the board
 */

void init_tables();
int eval(const Board& boardRef);
