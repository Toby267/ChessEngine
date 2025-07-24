#pragma once

#include "board/Board.hpp"
#include "board/BoardUtil.hpp"

/**
 * Class representing the engine as a whole
 * 
 * Handles game logic, I/O and communication between the user, bot, and board
 */
class Engine {
private:
    Board* board = new Board();
    WhiteTurn whiteTurn = true;
    int drawMoveCounter = 0;

public:
    //constructors/destructor
    Engine();
    ~Engine();

    //public methods
    void parseFen(const std::string& FEN);

private:
    //private methods
    void printASCIIBoard();
};