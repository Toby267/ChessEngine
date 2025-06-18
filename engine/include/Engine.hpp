#pragma once

#include "board/Board.hpp"

class Engine {
private:
    Board* board;

public:
    //constructors/destructor
    Engine();
    ~Engine();

private:
    //private methods
    void printASCIIBoard();
};