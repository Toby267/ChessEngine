#pragma once

#include "board/Board.hpp"

/**
 * Defines functions for a fast pesto evaluation of the board
 */

namespace Eval {
    extern const int CHEKMATE_ABSOLUTE_SCORE;
    
    void initPestoTables();
    int pestoEval(const Board& boardRef);
    int terminalNodeEval(const Board& boardRef);
}
