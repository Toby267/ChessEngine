#pragma once

#include "Bot.hpp"
#include "board/Board.hpp"
#include "board/BoardUtil.hpp"
#include "board/Move.hpp"
#include <bitset>
#include <queue>
#include <string>
#include <unordered_map>

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

    const WhiteTurn isBotWhite = false;
    GameState gameState = GameState::Live;

    std::unordered_map<std::bitset<64*14>, int> boardPositionCounter;

    std::queue<Move> previousMoves;
    
public:
    //constructors/destructor
    Engine();
    Engine(UserColour isBotWhite);
    ~Engine();

    //getters/setters

    //public methods
    void playMatch();
    void parseFen(const std::string& FEN);

private:
    //play match methods
    void printASCIIBoard();
    GameState getCurrentGameState();
    Move getUserMove();
    bool validateMove(Move& move, std::string moveString);
    
    //perft methods
    void runPerftTests(int rigor);
    uint64_t perft(int depth);
    uint64_t perftDivide(int depth);
};