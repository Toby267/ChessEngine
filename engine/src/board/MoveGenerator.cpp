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
std::vector<Move> generateMoves(Board& board, bool whiteTurn) {
    //assume its white turn then code for generalised
    std::vector<Move> moves;
    moves.reserve(32);

    const std::array<uint64_t, 14>& bitBoards = board.getBitBoards();

    //generate king moves
        //by calculation
    uint64_t pawnAttacks = bitBoards[PieceType::WHITE_KING];
    pawnAttacks |= eastOne(pawnAttacks) | westOne(pawnAttacks);
    pawnAttacks |= northOne(pawnAttacks) | southOne(pawnAttacks);

    printf("0x%016llx\n", bitBoards[PieceType::WHITE_KING]);
    printf("0x%016llx\n", pawnAttacks);

    //generate knight moves
        //multiple knight attacks parallel version

    uint64_t knights = bitBoards[PieceType::WHITE_KING];
    uint64_t l1 = (knights << 8 ) & (uint64_t)(0x0000FFFFFFFFFFFF);
    uint64_t l2 = (knights << 16) & (uint64_t)(0x00FFFFFFFFFFFFFF);
    uint64_t r1 = (knights >> 8 ) & (uint64_t)(0xFFFFFFFFFFFF0000);
    uint64_t r2 = (knights >> 16) & (uint64_t)(0xFFFFFFFFFFFFFF00);
    uint64_t h1 = l1 | r1;
    uint64_t h2 = l2 | r2;
    uint64_t knightAttacks = (h1<<2)&(0xFCFCFCFCFCFCFCFC) | (h1>>2)&(0x3F3F3F3F3F3F3F3F) | (h2<<1)&(0xFEFEFEFEFEFEFE) | (h2>>1)&(0x7F7F7F7F7F7F7F7F);
    //uint64_t knightAttacks = (h1<<2) | (h1>>2) | (h2<<1)&(0xFEFEFEFEFEFEFE) | (h2>>1)&(0x7F7F7F7F7F7F7F7F);

    // uint64_t west, east, attacks;
    // east     = eastOne (knights);
    // west     = westOne (knights);
    // attacks  = (east|west) >> 2;
    // attacks |= (east|west) << 2;
    // east     = eastOne (east);
    // west     = westOne (west);
    // attacks |= (east|west) >>  1;
    // attacks |= (east|west) <<  1;

    board.setBitBoard(knightAttacks, PieceType::BLACK_KING);
    board.setBitBoard(knightAttacks, PieceType::BLACK_PIECES);


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
