#include "bot/Bot.hpp"

#include <climits>
#include <cstring>
#include <iostream>
#include <vector>
#include <chrono>

#include "board/Board.hpp"
#include "board/Move.hpp"
#include "bot/PrincipalVariation.hpp"
#include "moveGeneration/MoveGenerator.hpp"
#include "bot/Eval.hpp"

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
        Eval::initPestoTables();
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

    for (int i = 1;; i++) {
        std::cout << "about to do negaMax(" << i << ')' << ", done negaMax(" << (i-1) << ')' << '\n';
        pVariation pvLine; //TODO: move this to the heap, to avoid stack overflow
        if (negaMax(i, -INT_MAX, INT_MAX, &pvLine) == Eval::CHEKMATE_ABSOLUTE_SCORE)
            return pvLine.moves[0];
        if (searchDeadlineReached)
            return principalVariation.moves[0];

        principalVariation = pvLine;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ---------------------------------------- [ PRIVATE METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Bot::negaMax(int depth, int alpha, int beta, pVariation* parentLine) {
    //timer for iterative deepening, checks at a certain frequency, and sets searchDeadlineReached to true once the deadline is reached
    if (searchDeadlineReached || (++nodesSearched % SEARCH_TIMER_NODE_FREQUENCY == 0 && checkTimer())) return beta; //effectively snipping this branch like in alpha-beta. could also return a heuristic for this move

    pVariation childLine;
    if (depth == 0) {
        parentLine->moveCount = 0;
        return Eval::pestoEval(board);
    }

    std::vector<Move> moves = MoveGeneration::generateMoves(board);
    if (!moves.size()) return Eval::terminalNodeEval(board);
    orderMoves(moves);

    for (Move move : moves) {
        board.makeMove(move);

        int eval = -negaMax(depth-1, -beta, -alpha, &childLine);
        
        board.unMakeMove(move);

        if (eval >= beta) {
            return beta;
        }
        if (eval > alpha) {
            alpha = eval;

            parentLine->moves[0] = move;
            memcpy(parentLine->moves+1, childLine.moves, childLine.moveCount * sizeof(Move));
            parentLine->moveCount = childLine.moveCount + 1;
        }
    }

    return alpha;
}

void Bot::orderMoves(std::vector<Move>& moves) {
    //TODO: move ordering based on pv tree
    std::sort(moves.begin(), moves.end(), [this](const Move& a, const Move& b){
        return a.heuristic > b.heuristic;
    });
}

bool Bot::checkTimer() {
    return (searchDeadlineReached = std::chrono::high_resolution_clock::now() > searchDeadline);
}
