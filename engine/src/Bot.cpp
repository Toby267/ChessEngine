#include "Bot.hpp"

#include <climits>
#include <iostream>
#include <vector>
#include <chrono>

#include "board/Board.hpp"
#include "board/Move.hpp"
#include "board/moveGeneration/MoveGenerator.hpp"
#include "evaluation/Pesto.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ STATIC MEMBERS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Bot::isPestoInitialised = false;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ------------------------------------ [ CONSTRUCTORS/DESCTUCTOR ] ------------------------------------ * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

Bot::Bot() : MAX_SEARCH_TIME_MS(60000), SEARCH_TIMER_NODE_FREQUENCY(1024) {
    if (!isPestoInitialised) {
        isPestoInitialised = true;
        initPestoTables();
    }
}
Bot::~Bot() {

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ PUBLIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Conducts searching and evaluation in order to find and return the predicted best possible move
 * given the current board state
 * 
 * @return the best move
 */
Move Bot::getBestMove(Board board) {    
    Move moves[2];

    auto start = std::chrono::high_resolution_clock::now();
    searchDeadline = MAX_SEARCH_TIME_MS + start;
    
    int i;
    for (i = 1;; i++) {
        std::cout << "about to do megamax(depth=" << i << ")" << '\n';
        std::cout << "adding to index: " << (i&1) << '\n';
        if (negaMax(moves[i&1], board, i) == CHEKMATE_ABSOLUTE_SCORE)
            break;
        if (searchDeadlineReached)
            break;
    }


    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time ellapsed (miliseconds): " << duration.count() << '\n';
    std::cout << "Leaf nodes evaluated: " << leafNodesEvaluated << '\n';

    std::cout << "returning index: " << (i&1) << '\n';
    return moves[!(i&1)];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ---------------------------------------- [ PRIVATE METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//depth >= 1
int Bot::negaMax(Move& moveRef, Board& board, int depth) {
    std::vector<Move> moves = generateMoves(board);
    int max = INT_MIN, maxIndex;

    for (int i = 0; i < moves.size(); i++) {
        board.makeMove(moves[i]);

        int eval = -negaMaxIter(board, depth - 1);
        if (eval > max) {
            max = eval;
            maxIndex = i;
        }

        board.unMakeMove(moves[i]);
    }

    moveRef = moves[maxIndex];
    return max;
}
int Bot::negaMaxIter(Board& board, int depth) {
    //timer for iterative deepening, checks at a certain frequency, and sets searchDeadlineReached to true once the deadline is reached
    if (searchDeadlineReached || (++nodesSearched % SEARCH_TIMER_NODE_FREQUENCY == 0 && checkTimer())) return -CHEKMATE_ABSOLUTE_SCORE; //haven't finished this move, so assume its bad
    
    if (depth == 0) return pestoEval(board);

    std::vector<Move> moves = generateMoves(board);
    if (!moves.size()) return terminalNodeEval(board);
    int max = INT_MIN;

    for (Move move : moves) {
        board.makeMove(move);

        int eval = -negaMaxIter(board, depth - 1);
        if (eval > max)
            max = eval;

        board.unMakeMove(move);
    }

    return max;
}

bool Bot::checkTimer() {
    return (searchDeadlineReached = std::chrono::high_resolution_clock::now() > searchDeadline);
}
