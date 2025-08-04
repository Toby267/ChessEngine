#pragma once

#include "board/Board.hpp"
#include "board/BoardUtil.hpp"
#include "board/Move.hpp"
#include <string>

enum gameState {
    Live,
    Checkmate,
    Stalemate
};

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
    void playMatch();
    void parseFen(const std::string& FEN);

private:
    //private methods
    void printASCIIBoard();

    Move getUserMove();
    bool validateMove(Move& move, std::string moveString);

    gameState getCurrentGameState();

    uint64_t perft(int depth);
    uint64_t perftDivide(int depth);
    void runPerftTests();
};