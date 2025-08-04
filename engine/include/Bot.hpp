#pragma once

#include "board/Board.hpp"
#include "board/Move.hpp"

class Bot {
private:
    Board& board;

public:
    //constructors/destructor
    Bot(Board& board);
    ~Bot();

    Move getBestMove();

private:

};