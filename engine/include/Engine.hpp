#pragma once

#include "Bot.hpp"
#include "board/Board.hpp"
#include "board/BoardUtil.hpp"
#include "board/Move.hpp"
#include <bitset>
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
    Bot* bot = new Bot();

    const WhiteTurn isBotWhite = false;
    GameState gameState = GameState::Live;

    std::unordered_map<std::bitset<64*14>, int> boardPositionCounter;

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
    //private methods
    void printASCIIBoard();

    GameState getCurrentGameState();
    Move getUserMove();
    
    void runPerftTests(int rigor);

    //helper methods
    bool validateMove(Move& move, std::string moveString);

    uint64_t perft(int depth);
    uint64_t perftDivide(int depth);
};