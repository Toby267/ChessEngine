#include "board/MoveGenerator.hpp"

#include <vector>
#include <cstdint>
#include <array>
#include <iostream>

#include "board/Board.hpp"
#include "board/Move.hpp"

// * ---------------------------------- [ PUBLIC METHODS ] ----------------------------------- * //

/**
 * Generates all possible moves based on a given board and whos to move
 * 
 * @param board the board
 * @param whiteTurn whether or not it is whites turn to move
 */
std::vector<Move> generateMoves(const Board& board, bool whiteTurn) {
    //assume its white turn then code for generalised
    std::cout << "ooga booga" << '\n';
    
    std::vector<Move> moves;
    moves.reserve(32);

    const std::array bitBoards = board.getBitBoards();

    //generate king moves
        //by calculation
    uint64_t attacks = bitBoards[PieceType::WHITE_KING];
    attacks |= eastOne(attacks) | westOne(attacks);
    attacks |= northOne(attacks) | southOne(attacks);

    printf("0x%016llx\n", bitBoards[PieceType::WHITE_KING]);
    printf("0x%016llx\n", attacks);

    


    //generate knight moves
        //multiple knight attacks parallel version
    //generate pawn moves
        //push per side
        //pawns able to push
        
        //pawns set-wise
        //captures

    //generate rook moves
    //generate bishop moves
    //generate queen moves

    return moves;
}

/**
 * 
 */
void bitScan() {

}
