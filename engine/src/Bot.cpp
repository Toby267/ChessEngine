#include "Bot.hpp"

#include <climits>
#include <cstring>
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

Bot::Bot(Board& board) : board(board), MAX_SEARCH_TIME_MS(30000), SEARCH_TIMER_NODE_FREQUENCY(1024) {
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
Move Bot::getBestMove() {
    nodesSearched = 0;
    searchDeadlineReached = false;
    searchDeadline = MAX_SEARCH_TIME_MS + std::chrono::high_resolution_clock::now();
    
    pVariation pvLists[2];

    for (int i = 1;; i++) {
        // std::cout << "about to do negaMax(" << i << ')' << ", done negaMax(" << (i-1) << ')' << '\n';
        if (negaMax(i, -INT_MAX, INT_MAX, &pvLists[i&1]) == CHEKMATE_ABSOLUTE_SCORE)
            return pvLists[i&1].moves[0];
        if (searchDeadlineReached)
            return pvLists[!(i&1)].moves[0];
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ---------------------------------------- [ PRIVATE METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Bot::negaMax(int depth, int alpha, int beta, pVariation* pline) {
    //timer for iterative deepening, checks at a certain frequency, and sets searchDeadlineReached to true once the deadline is reached
    if (searchDeadlineReached || (++nodesSearched % SEARCH_TIMER_NODE_FREQUENCY == 0 && checkTimer())) return beta; //effectively snipping this branch like in alpha-beta. could also return a heuristic for this move

    pVariation line;
    if (depth == 0) {
        pline->moveCount = 0;
        return pestoEval(board);
    }

    std::vector<Move> moves = generateMoves(board);
    if (!moves.size()) return terminalNodeEval(board);

    for (Move move : moves) {
        board.makeMove(move);

        int eval = -negaMax(depth-1, -beta, -alpha, &line);
        
        board.unMakeMove(move);

        if (eval >= beta) {
            return beta;
        }
        if (eval > alpha) {
            alpha = eval;

            pline->moves[0] = move;
            memcpy(pline->moves+1, line.moves, line.moveCount * sizeof(Move));
            pline->moveCount = line.moveCount + 1;
        }
    }

    return alpha;
}

bool Bot::checkTimer() {
    return (searchDeadlineReached = std::chrono::high_resolution_clock::now() > searchDeadline);
}
