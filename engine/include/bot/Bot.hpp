#pragma once

#include "board/Board.hpp"
#include "board/Move.hpp"
#include "bot/PrincipalVariation.hpp"

#include <chrono>
#include <semaphore>

/**
 * Class representing the Bot and its relevent data/ references.
 * 
 * Has one main function getBestMove() used to conduct searching and evaluation methods
 * and return the predicted best move
 */
class Bot {
private:
    static bool isPestoInitialised;
    static const std::string OPENING_BOOKS[];
    static const int NUM_THREADS;
    static std::counting_semaphore<> threadsAvailable;

    Board& board;
    pVariation principalVariation; //could this just be a vector?

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
    int negaMax(int depth, int alpha, int beta, pVariation& parentLine);
    int negaMaxConcurrent(int depth, int alpha, int beta, pVariation& parentLine, Board b);
    int quiescence(int alpha, int beta);
    bool queryOpeningBook(std::string bookName, Move& move);

    //helper methods
    void orderMoves(std::vector<Move>& moves);
    void orderMovesQuiescence(std::vector<Move>& moves);
    bool checkTimer();
};
