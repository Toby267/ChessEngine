#pragma once

#include "bot/Bot.hpp"
#include "board/Board.hpp"
#include "board/BoardUtil.hpp"
#include "board/Move.hpp"
#include <future>
#include <queue>
#include <string>

typedef bool UserColour;

enum GameState {
    Live,
    Checkmate,
    Stalemate,
    DrawByRepetition
};

/**
 * Class representing the engine as a whole
 * 
 * Handles game logic, I/O and communication between the user, bot, and board
 */
class Engine {
private:
    Board* board = new Board();
    Bot* bot;

    WhiteTurn isBotWhite = false;
    GameState gameState = GameState::Live;

    int blackTimeMs = 0;
    int whiteTimeMs = 0;

    std::queue<Move> previousMoves;

    std::future<Move> bestMove;
    
public:
    //constructors/destructor
    Engine();
    ~Engine();

private:
    //uci
    void parseCommand(std::string command);
    void parseGoCommand(std::string command);
    void parsePositionCommand(std::string command);

    //play match methods
    void playMatch();
    void printASCIIBoard();
    GameState getCurrentGameState();
    Move getUserMove();
    bool validateMove(Move& move, std::string moveString);
    
    //perft methods
    void runPerftTests(int rigor);
    uint64_t perft(int depth);
    uint64_t perftDivide(int depth);
};