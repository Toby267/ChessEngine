#include "Bot.hpp"

#include <climits>
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

Bot::Bot() : MAX_SEARCH_TIME_MS(30000), SEARCH_TIMER_NODE_FREQUENCY(1024) {
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

    nodesSearched = 0;
    searchDeadlineReached = false;
    searchDeadline = MAX_SEARCH_TIME_MS + std::chrono::high_resolution_clock::now();
    
    int i;
    for (i = 1;; i++) {
        // std::cout << "about to do negaMax(" << i << ')' << ", completed negaMax(" << i-1 << ')' << '\n';
        if (negaMax(moves[i&1], board, i, -INT_MAX, INT_MAX) == CHEKMATE_ABSOLUTE_SCORE)
            break;
        if (searchDeadlineReached)
            break;
    }

    return moves[!(i&1)];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ---------------------------------------- [ PRIVATE METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//depth >= 1
int Bot::negaMax(Move& moveRef, Board& board, int depth, int alpha, int beta) {
    std::vector<Move> moves = generateMoves(board);
    int maxIndex = -1;

    for (int i = 0; i < moves.size(); i++) {
        board.makeMove(moves[i]);

        int eval = -negaMaxIter(board, depth-1, -beta, -alpha);
        
        board.unMakeMove(moves[i]);

        if (eval >= beta) {
            moveRef = moves[i];
            return beta;
        }
        if (eval > alpha) {
            alpha = eval;
            maxIndex = i;
        }
    }

    if (maxIndex != -1) moveRef = moves[maxIndex];
    return alpha;
}
int Bot::negaMaxIter(Board& board, int depth, int alpha, int beta) {
    //timer for iterative deepening, checks at a certain frequency, and sets searchDeadlineReached to true once the deadline is reached
    if (searchDeadlineReached || (++nodesSearched % SEARCH_TIMER_NODE_FREQUENCY == 0 && checkTimer())) return beta; //effectively snipping this branch like in alpha-beta. could also return a heuristic for this move
    
    if (depth == 0) return pestoEval(board);

    std::vector<Move> moves = generateMoves(board);
    if (!moves.size()) return terminalNodeEval(board);

    for (Move move : moves) {
        board.makeMove(move);

        int eval = -negaMaxIter(board, depth-1, -beta, -alpha);
        
        board.unMakeMove(move);

        if (eval >= beta) {
            return beta;
        }
        if (eval > alpha)
            alpha = eval;
    }

    return alpha;
}

bool Bot::checkTimer() {
    return (searchDeadlineReached = std::chrono::high_resolution_clock::now() > searchDeadline);
}
