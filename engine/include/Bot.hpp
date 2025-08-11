#pragma once

#include "board/Board.hpp"
#include "board/Move.hpp"
#include <chrono>

/**
 * Class representing the Bot and its relevent data/ references.
 * 
 * Has one main function getBestMove() used to conduct searching and evaluation methods
 * and return the predicted best move
 */
class Bot {
private:
    static bool isPestoInitialised;

    const std::chrono::milliseconds MAX_SEARCH_TIME_MS;
    const int SEARCH_TIMER_NODE_FREQUENCY;

    std::chrono::high_resolution_clock::time_point searchDeadline;
    int nodesSearched = 0;
    bool searchDeadlineReached = false;

public:
    //constructors/destructor
    Bot();
    ~Bot();

    //getters/setters

    //public methods
    Move getBestMove(Board board);

private:
    //private methods
    int negaMax(Move& moveRef, Board& board, int depth);
    int negaMaxIter(Board& board, int depth);
    
    //helper methods
    bool checkTimer();
};
