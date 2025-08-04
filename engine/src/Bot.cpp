#include "Bot.hpp"

#include <cstdlib>
#include <vector>
#include <ctime>

#include "board/moveGeneration/MoveGenerator.hpp"

// * ---------------------------------- [ CONSTRUCTORS/DESCTUCTOR ] ---------------------------------- * //

Bot::Bot(Board& board) : board(board) {
    
}
Bot::~Bot() {

}

// * ---------------------------------- [ PUBLIC METHODS ] ----------------------------------- * //

Move Bot::getBestMove() {
    srand(time(nullptr));

    std::vector<Move> moves = generateMoves(board);
    int i = rand() % moves.size();
    
    return moves[i];
}