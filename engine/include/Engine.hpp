#pragma once

#include "board/Board.hpp"

class Engine {
private:
    Board *board;

public:
    //constructors/destructorss
    Engine();
    ~Engine();

private:
    //private methods
    void printASCIIBoard();
};