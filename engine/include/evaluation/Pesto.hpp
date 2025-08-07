#pragma once

#include "board/Board.hpp"

/**
 * Defines functions for a fast pesto evaluation of the board
 */

void initPestoTables();
int pestoEval(const Board& boardRef);
