#include "board/MoveGenerator.hpp"

#include <iostream>

#include <vector>
#include <cstdint>

#include "board/Board.hpp"
#include "board/Move.hpp"
#include "board/BoardUtil.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ STATIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

static uint64_t generateKingMoves(uint64_t king, uint64_t friendlyPieces);
static uint64_t generateKnightMoves(uint64_t knights, uint64_t friendlyPieces);

static uint64_t generatePawnPushesWhite(uint64_t pawns, uint64_t unoccupied);
static uint64_t generatePawnPushesBlack(uint64_t pawns, uint64_t unoccupied);
static uint64_t generatePawnAttacksWhite(uint64_t pawns, uint64_t blackPieces);
static uint64_t generatePawnAttacksBlack(uint64_t pawns, uint64_t whitePieces);

static uint64_t generateRookMoves(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces);
static uint64_t generateBishopMoves(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces);
static uint64_t generateQueenMoves(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces);

static uint64_t getPositiveRay(SquareIndex square, uint64_t occupied, Direction dir);
static uint64_t getNegativeRay(SquareIndex square, uint64_t occupied, Direction dir);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ PUBLIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Generates all possible moves based on a given board and whos to move
 * 
 * @param board the board
 * @param whiteTurn whether or not it is whites turn to move
 */
std::vector<Move> generateMoves(Board& board, bool whiteTurn, SquareIndex temp) {//todo: remove the temp variable
    //return vector containing all moves converted into type Move.
    std::cout << whiteTurn << '\n';

    std::vector<Move> moves;
    moves.reserve(32);

    //constant values including the bitboards and masks
    const std::array<uint64_t, 14>& bitBoards           = board.getBitBoards();
    const uint64_t                  whitePieces         = bitBoards[PieceType::WHITE_PIECES];
    const uint64_t                  blackPieces         = bitBoards[PieceType::BLACK_PIECES];
    const uint64_t                  friendlyPieces      = whiteTurn ? whitePieces : blackPieces;
    const uint64_t                  oppositionPieces    = whiteTurn ? blackPieces : whitePieces;
    const uint64_t                  occupied            = whitePieces | blackPieces;
    const uint64_t                  unoccupied          = ~occupied;
    const short                     indexOffset         = whiteTurn ? 0 : 6;

    //generate moves for the easy pieces
    uint64_t kingMoves      = generateKingMoves(bitBoards[PieceType::WHITE_KING + indexOffset], friendlyPieces);
    uint64_t knightMoves    = generateKnightMoves(bitBoards[PieceType::WHITE_KNIGHT + indexOffset], friendlyPieces);

    //generate moves for pawns
    //still need to test pawn moves
    uint64_t pawnPushes     = whiteTurn ?   generatePawnPushesWhite(bitBoards[PieceType::WHITE_PAWN], unoccupied):
                                            generatePawnPushesBlack(bitBoards[PieceType::BLACK_PAWN], unoccupied);
    uint64_t pawnAttacks    = whiteTurn ?   generatePawnAttacksWhite(bitBoards[PieceType::WHITE_PAWN], blackPieces):
                                            generatePawnAttacksBlack(bitBoards[PieceType::BLACK_PAWN], whitePieces);

    //generate moves for sliding pieces
    //need to extract rook, bishop, & queen positions before putting them into this
    //need to also finish the functinality with the unoccupied mask
    uint64_t rookMoves      = generateRookMoves(temp, occupied, friendlyPieces);
    uint64_t bishopMoves    = generateBishopMoves(temp, occupied, friendlyPieces);
    uint64_t queenMoves     = generateQueenMoves(temp, occupied, friendlyPieces);

    //generate en passant and castling

    board.resetBoard();
    board.setBitBoard(queenMoves, PieceType::BLACK_KING);
    board.setBitBoard(queenMoves, PieceType::BLACK_PIECES);

    //finally return
    return moves;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ---------------------------------------- [ PRIVATE METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// * ---------------------------------------- [ EASY MOVES ] ---------------------------------------- * //

static uint64_t generateKingMoves(uint64_t king, uint64_t friendlyPieces) {
    //only checks for legal postitions by valid move directions, empty squares  borders, doesn't check checks
    uint64_t kingMoves = king;
    kingMoves |= eastOne(kingMoves)  | westOne(kingMoves);
    kingMoves |= northOne(kingMoves) | southOne(kingMoves);
    return kingMoves & ~friendlyPieces;
}

static uint64_t generateKnightMoves(uint64_t knights, uint64_t friendlyPieces) {
    //only checks for legal postitions by valid move directions, empty squares & borders, doesn't check checks
    uint64_t h1 = (knights << 8 ) | (knights >> 8 );
    uint64_t h2 = (knights << 16) | (knights >> 16);
    uint64_t knightMoves =  (h1<<2) & (0xFCFCFCFCFCFCFCFCULL) |
                            (h1>>2) & (0x3F3F3F3F3F3F3F3FULL) |
                            (h2<<1) & (0xFEFEFEFEFEFEFEFEULL) |
                            (h2>>1) & (0x7F7F7F7F7F7F7F7FULL) ;
    return knightMoves & ~friendlyPieces;
}

// * ---------------------------------------- [ PAWN MOVES ] ---------------------------------------- * //

static uint64_t generatePawnPushesWhite(uint64_t pawns, uint64_t unoccupied) {
    //only checks for legal postitions by valid move directions, empty squares & borders, doesn't check checks
    uint64_t singlePushes = northOne(pawns) & unoccupied;
    uint64_t doublePushes = northOne(singlePushes) & unoccupied & 0x0808080808080808ULL;
    return singlePushes | doublePushes;
}

static uint64_t generatePawnPushesBlack(uint64_t pawns, uint64_t unoccupied) {
    //only checks for legal postitions by valid move directions, empty squares & borders, doesn't check checks
    uint64_t singlePushes = southOne(pawns) & unoccupied;
    uint64_t doublePushes = southOne(singlePushes) & unoccupied & 0x1010101010101010ULL;
    return singlePushes | doublePushes;
}
static uint64_t generatePawnAttacksWhite(uint64_t pawns, uint64_t blackPieces) {
    //only checks for legal postitions by valid move directions, empty squares & borders, doesn't check checks
    uint64_t moves = northEastOne(pawns) | northWestOne(pawns);
    return moves & blackPieces;
}

static uint64_t generatePawnAttacksBlack(uint64_t pawns, uint64_t whitePieces) {
    //only checks for legal postitions by valid move directions, empty squares & borders, doesn't check checks
    uint64_t moves = southEastOne(pawns) | southWestOne(pawns);
    return moves & whitePieces;
}

// * ---------------------------------------- [ SLIDING MOVES ] ---------------------------------------- * //

static uint64_t generateRookMoves(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces) {
    //only checks for legal postitions by valid move directions, empty squares & borders, although it cant take other pieces yet as it counts them as taken spots; doesn't check checks
    uint64_t moves =    getPositiveRay(square, occupied, Direction::NORTH)  |
                        getPositiveRay(square, occupied, Direction::EAST)   |
                        getNegativeRay(square, occupied, Direction::SOUTH)  |
                        getNegativeRay(square, occupied, Direction::WEST)   ;

    return moves & ~friendlyPieces;
}

static uint64_t generateBishopMoves(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces) {
    //only checks for legal postitions by valid move directions, empty squares & borders, although it cant take other pieces yet as it counts them as taken spots; doesn't check checks
    uint64_t moves =    getNegativeRay(square, occupied, Direction::SOUTH_WEST) |
                        getPositiveRay(square, occupied, Direction::NORTH_EAST) |
                        getPositiveRay(square, occupied, Direction::SOUTH_EAST) |
                        getNegativeRay(square, occupied, Direction::NORTH_WEST) ;

    return moves & ~friendlyPieces;
}

static uint64_t generateQueenMoves(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces) {
    //only checks for legal postitions by valid move directions, empty squares & borders, although it cant take other pieces yet as it counts them as taken spots; doesn't check checks
    uint64_t moves =    getPositiveRay(square, occupied, Direction::NORTH)      |
                        getPositiveRay(square, occupied, Direction::EAST)       |
                        getNegativeRay(square, occupied, Direction::SOUTH)      |
                        getNegativeRay(square, occupied, Direction::WEST)       |
                        getNegativeRay(square, occupied, Direction::SOUTH_WEST) |
                        getPositiveRay(square, occupied, Direction::NORTH_EAST) |
                        getPositiveRay(square, occupied, Direction::SOUTH_EAST) |
                        getNegativeRay(square, occupied, Direction::NORTH_WEST) ;

    return moves & ~friendlyPieces;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ HELPER METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

static uint64_t getPositiveRay(SquareIndex square, uint64_t occupied, Direction dir) {
    uint64_t ray = rayFunctions[dir](square);
    uint64_t blockers = ray & occupied;
    int firstBlocker = __builtin_ctzll(blockers | 0x8000000000000000ULL);//same as (1ULL << 63)
    ray ^= rayFunctions[dir]((SquareIndex)firstBlocker);
    return ray;
}

static uint64_t getNegativeRay(SquareIndex square, uint64_t occupied, Direction dir) {
    uint64_t ray = rayFunctions[dir](square);
    uint64_t blockers = ray & occupied;
    int firstBlocker = 63 - __builtin_clzll(blockers | 1);
    ray ^= rayFunctions[dir]((SquareIndex)firstBlocker);
    return ray;
}
