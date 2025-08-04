#pragma once

#include "Bot.hpp"
#include "board/Board.hpp"
#include "board/BoardUtil.hpp"
#include "board/Move.hpp"
#include <string>

typedef bool UserColour;

enum GameState {
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
    Bot* bot = new Bot(*board);

    const WhiteTurn isBotWhite = false;
    int drawMoveCounter = 0;

public:
    //constructors/destructor
    Engine();
    Engine(UserColour isBotWhite);
    ~Engine();

    //public methods
    void playMatch();
    void parseFen(const std::string& FEN);

private:
    //private methods
    void printASCIIBoard();

    Move getUserMove();
    bool validateMove(Move& move, std::string moveString);

    GameState getCurrentGameState();

    uint64_t perft(int depth);
    uint64_t perftDivide(int depth);
    void runPerftTests();
};