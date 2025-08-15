#pragma once

#include "board/Board.hpp"
#include "board/Move.hpp"
#include <chrono>

typedef struct pVariation {
    int moveCount;
    Move moves[15];
} pVariation;

/**
 * Class representing the Bot and its relevent data/ references.
 * 
 * Has one main function getBestMove() used to conduct searching and evaluation methods
 * and return the predicted best move
 */
class Bot {
private:
    static bool isPestoInitialised;

    Board& board;

    const std::chrono::milliseconds MAX_SEARCH_TIME_MS;
    const int SEARCH_TIMER_NODE_FREQUENCY;

    std::chrono::high_resolution_clock::time_point searchDeadline;
    int nodesSearched = 0;
    bool searchDeadlineReached = false;

public:
    //constructors/destructor
    Bot(Board& board);
    ~Bot();

    //getters/setters

    //public methods
    Move getBestMove();

private:
    //private methods
    int negaMax(int depth, int alpha, int beta, pVariation* pline);
    
    //helper methods
    bool checkTimer();
};
