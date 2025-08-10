#include "Bot.hpp"

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
    Move move;
    
    for (int i = 1; i <= 5; i++)
        if (negaMax(move, board, i) == 9999999)
            break;

    return move;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ---------------------------------------- [ PRIVATE METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//depth >= 1
int Bot::negaMax(Move& moveRef, Board& board, int depth) {
    std::vector<Move> moves = generateMoves(board);
    int max = -9999999, maxIndex;

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
    if (depth == 0) return pestoEval(board);

    std::vector<Move> moves = generateMoves(board);
    if (!moves.size()) return terminalNodeEval(board);
    int max = -9999999;

    for (Move move : moves) {
        board.makeMove(move);

        int eval = -negaMaxIter(board, depth - 1);
        if (eval > max)
            max = eval;

        board.unMakeMove(move);
    }

    return max;
}
