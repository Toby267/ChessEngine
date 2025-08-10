#pragma once

#include "board/Board.hpp"
#include "board/Move.hpp"

/**
 * Class representing the Bot and its relevent data/ references.
 * 
 * Has one main function getBestMove() used to conduct searching and evaluation methods
 * and return the predicted best move
 */
class Bot {
private:
    static bool isPestoInitialised;

public:
    //constructors/destructor
    Bot();
    ~Bot();

    //getters/setters

    //public methods
    Move getBestMove(Board& board);

private:
    //private methods
    int negaMax(Move& moveRef, Board& board, int depth);
    int negaMaxIter(Board& board, int depth);
    
    //helper methods
};
