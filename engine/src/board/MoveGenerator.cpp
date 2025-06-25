#include "board/MoveGenerator.hpp"

#include <vector>
#include <cstdint>

#include "board/Board.hpp"
#include "board/Move.hpp"

static uint64_t generateKingMoves(uint64_t king, uint64_t notFriendly);
static uint64_t generateKnightMoves(uint64_t knights, uint64_t notFriendly);

static uint64_t generatePawnPushesWhite(uint64_t pawns, uint64_t unoccupied);
static uint64_t generatePawnPushesBlack(uint64_t pawns, uint64_t unoccupied);
static uint64_t generatePawnAttacksWhite(uint64_t pawns, uint64_t blackPieces);
static uint64_t generatePawnAttacksBlack(uint64_t pawns, uint64_t whitePieces);

static uint64_t generateRookMoves(SquareIndex square, uint64_t occupied);
static uint64_t generateBishopMoves(SquareIndex square, uint64_t occupied);
static uint64_t generateQueenMoves(SquareIndex square, uint64_t occupied);

// * ---------------------------------- [ STATIC ATTRIBUTES ] ----------------------------------- * //

static uint64_t** slidingAttacks = {};

// * ---------------------------------- [ PUBLIC METHODS ] ----------------------------------- * //

/**
 * Generates all possible moves based on a given board and whos to move
 * 
 * @param board the board
 * @param whiteTurn whether or not it is whites turn to move
 */
std::vector<Move> generateMoves(Board& board, bool whiteTurn) {
    //return vector containing all moves converted into type Move.
    std::vector<Move> moves;
    moves.reserve(32);

    //constant values including the bitboards and masks
    const std::array<uint64_t, 14>& bitBoards = board.getBitBoards();
    const uint64_t  whitePieces = bitBoards[PieceType::WHITE_PIECES];
    const uint64_t  blackPieces = bitBoards[PieceType::BLACK_PIECES];
    const uint64_t  notFriendly = whiteTurn ? ~whitePieces : ~blackPieces;
    const uint64_t  occupied    = whitePieces | blackPieces;
    const uint64_t  unoccupied  = ~occupied;
    const short     indexOffset = whiteTurn ? 0 : 6;

    //generate moves for the easy pieces
    uint64_t kingMoves      = generateKingMoves(bitBoards[PieceType::WHITE_KING + indexOffset], notFriendly);
    uint64_t knightMoves    = generateKnightMoves(bitBoards[PieceType::WHITE_KNIGHT + indexOffset], notFriendly);

    //generate moves for pawns
    uint64_t pawnPushes     = whiteTurn ?   generatePawnPushesWhite(bitBoards[PieceType::WHITE_PAWN], unoccupied):
                                            generatePawnPushesBlack(bitBoards[PieceType::BLACK_PAWN], unoccupied);
    uint64_t pawnAttacks    = whiteTurn ?   generatePawnAttacksWhite(bitBoards[PieceType::WHITE_PAWN], blackPieces):
                                            generatePawnAttacksBlack(bitBoards[PieceType::BLACK_PAWN], whitePieces);

    //generate moves for sliding pieces
    //need to extract rook, bishop, & queen positions before putting them into this
    //need to also finish the functinality with the unoccupied mask
    uint64_t rookMoves      = generateRookMoves(SquareIndex::d4, unoccupied);
    uint64_t bishopMoves    = generateBishopMoves(SquareIndex::d4, unoccupied);
    uint64_t queenMoves     = generateQueenMoves(SquareIndex::d4, unoccupied);

    //generate en passant and castling

    board.setBitBoard(queenMoves, PieceType::BLACK_KING);
    board.setBitBoard(queenMoves, PieceType::BLACK_PIECES);

    //finally return
    return moves;
}

// * ---------------------------------- [ PRIVATE METHODS ] ---------------------------------- * //

static uint64_t generateKingMoves(uint64_t king, uint64_t notFriendly) {
    //only checks for legal postitions by valid move directions, empty squares  borders, doesn't check checks
    uint64_t kingMoves = king;
    kingMoves |= eastOne(kingMoves)  | westOne(kingMoves);
    kingMoves |= northOne(kingMoves) | southOne(kingMoves);
    return kingMoves & notFriendly;
}

static uint64_t generateKnightMoves(uint64_t knights, uint64_t notFriendly) {
    //only checks for legal postitions by valid move directions, empty squares & borders, doesn't check checks
    uint64_t h1 = (knights << 8 ) | (knights >> 8 );
    uint64_t h2 = (knights << 16) | (knights >> 16);
    uint64_t knightMoves =  (h1<<2) & (0xFCFCFCFCFCFCFCFC) |
                            (h1>>2) & (0x3F3F3F3F3F3F3F3F) |
                            (h2<<1) & (0xFEFEFEFEFEFEFEFE) |
                            (h2>>1) & (0x7F7F7F7F7F7F7F7F) ;
    return knightMoves & notFriendly;
}

//TODO: test this
static uint64_t generatePawnPushesWhite(uint64_t pawns, uint64_t unoccupied) {
    //only checks for legal postitions by valid move directions, empty squares & borders, doesn't check checks
    uint64_t singlePushes = northOne(pawns) & unoccupied;
    uint64_t doublePushes = northOne(singlePushes) & unoccupied & 0x0808080808080808;
    return singlePushes | doublePushes;
}
//TODO: test this
static uint64_t generatePawnPushesBlack(uint64_t pawns, uint64_t unoccupied) {
    //only checks for legal postitions by valid move directions, empty squares & borders, doesn't check checks
    uint64_t singlePushes = southOne(pawns) & unoccupied;
    uint64_t doublePushes = southOne(singlePushes) & unoccupied & 0x1010101010101010;
    return singlePushes | doublePushes;
}

//TODO: test this
static uint64_t generatePawnAttacksWhite(uint64_t pawns, uint64_t blackPieces) {
    //only checks for legal postitions by valid move directions, empty squares & borders, doesn't check checks
    uint64_t moves = northEastOne(pawns) | northWestOne(pawns);
    return moves & blackPieces;
}
//TODO: test this
static uint64_t generatePawnAttacksBlack(uint64_t pawns, uint64_t whitePieces) {
    //only checks for legal postitions by valid move directions, empty squares & borders, doesn't check checks
    uint64_t moves = southEastOne(pawns) | southWestOne(pawns);
    return moves & whitePieces;
}

static uint64_t generateRookMoves(SquareIndex square, uint64_t occupied) {
    //TODO: finish this - need to use the mask in someway - probably manipulate it someway before ORing together
    uint64_t mask = calcNorthMask(square) | calcEastMask(square) | calcSouthMask(square) | calcWestMask(square);
    return mask;
}
static uint64_t generateBishopMoves(SquareIndex square, uint64_t occupied) {
    //TODO: finish this - need to use the mask in someway - probably manipulate it someway before ORing together
    uint64_t mask = calcNorthEastMask(square) | calcSouthEastMask(square) | calcSouthWestMask(square) | calcNorthWestMask(square);
    return mask;
}
static uint64_t generateQueenMoves(SquareIndex square, uint64_t occupied) {
    //TODO: finish this - need to use the mask in someway - probably manipulate it someway before ORing together
    uint64_t mask = generateRookMoves(square, occupied) | generateBishopMoves(square, occupied);
    return mask;
}
