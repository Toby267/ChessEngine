#pragma once

#include "board/Board.hpp"

class Engine {
private:
    Board* board;
    bool whiteTurn = true;
    short drawMoveCounter = 0;

public:
    //constructors/destructor
    Engine();
    ~Engine();

    void parseFen(const std::string& FEN);

private:
    //private methods
    void printASCIIBoard();
};