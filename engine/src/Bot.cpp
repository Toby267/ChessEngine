#include "Bot.hpp"

#include <climits>
#include <vector>

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

Bot::Bot() {
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
Move Bot::getBestMove(Board& board) {
    return negaMax(board, 5);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ---------------------------------------- [ PRIVATE METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//depth >= 1
Move Bot::negaMax(Board& board, int depth) {
    std::vector<Move> moves = generateMoves(board);
    int max = INT_MIN, maxIndex;

    for (int i = 0; i < moves.size(); i++) {
        board.makeMove(moves[i]);

        int eval = negaMaxIter(board, depth - 1);
        
        if (eval > max) {
            max = eval;
            maxIndex = i;
        }

        board.unMakeMove(moves[i]);
    }

    return moves[maxIndex];
}
int Bot::negaMaxIter(Board& board, int depth) {
    if (depth == 0) return pestoEval(board);

    std::vector<Move> moves = generateMoves(board);
    int max = INT_MIN;

    for (Move move : moves) {
        board.makeMove(move);

        int eval = negaMaxIter(board, depth - 1);
        if (eval > max)
            max = eval;

        board.unMakeMove(move);
    }

    return max;
}
