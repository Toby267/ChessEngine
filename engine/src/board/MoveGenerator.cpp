#include "board/MoveGenerator.hpp"

#include <vector>
#include <cstdint>

#include "board/Board.hpp"
#include "board/Move.hpp"

static uint64_t generateKingMoves(uint64_t king, uint64_t unoccupied);
static uint64_t generateKnightMoves(uint64_t knights, uint64_t unoccupied);
static uint64_t generatePawnPushesWhite(uint64_t pawns, uint64_t empty);
static uint64_t generatePawnPushesBlack(uint64_t pawns, uint64_t empty);
static uint64_t generatePawnAttacksWhite(uint64_t pawns, uint64_t blackPieces);
static uint64_t generatePawnAttacksBlack(uint64_t pawns, uint64_t whitePieces);

// * ---------------------------------- [ PUBLIC METHODS ] ----------------------------------- * //

/**
 * Generates all possible moves based on a given board and whos to move
 * 
 * @param board the board
 * @param whiteTurn whether or not it is whites turn to move
 */
std::vector<Move> generateMoves(Board& board, bool whiteTurn) {
    //yet to do castling and en passant
    std::vector<Move> moves;
    moves.reserve(32);

    const std::array<uint64_t, 14>& bitBoards = board.getBitBoards();

    const uint64_t whitePieces = bitBoards[PieceType::WHITE_PIECES];
    const uint64_t blackPieces = bitBoards[PieceType::BLACK_PIECES];
    const uint64_t occupied = whiteTurn ? ~whitePieces : ~blackPieces;
    const uint64_t empty    = ~(whitePieces | blackPieces);

    //generate king moves
    uint64_t kingMoves = generateKingMoves(bitBoards[PieceType::WHITE_KING], occupied);
    //generate knight moves
    uint64_t knightMoves = generateKnightMoves(bitBoards[PieceType::WHITE_KNIGHT], occupied);

    //generate pawn moves
    uint64_t pawnPushes = whiteTurn ?   generatePawnPushesWhite(bitBoards[PieceType::WHITE_PAWN], empty) :
                                        generatePawnPushesBlack(bitBoards[PieceType::BLACK_PAWN], empty) ;

    uint64_t pawnAttacks = whiteTurn ?  generatePawnAttacksWhite(bitBoards[PieceType::WHITE_PAWN], blackPieces) :
                                        generatePawnAttacksBlack(bitBoards[PieceType::BLACK_PAWN], whitePieces) ;
        
        //pawns set-wise
        //captures

    //generate rook moves
    //generate bishop moves
    //generate queen moves


    board.setBitBoard(pawnAttacks, PieceType::BLACK_KING);
    board.setBitBoard(pawnAttacks, PieceType::BLACK_PIECES);

    return moves;
}

// * ---------------------------------- [ PRIVATE METHODS ] ---------------------------------- * //

static uint64_t generateKingMoves(uint64_t king, uint64_t unoccupied) {
    //only checks for legal postitions by valid move directions, empty squares  borders, doesn't check checks
    uint64_t kingMoves = king;
    kingMoves |= eastOne(kingMoves)  | westOne(kingMoves);
    kingMoves |= northOne(kingMoves) | southOne(kingMoves);
    return kingMoves & unoccupied;
}

static uint64_t generateKnightMoves(uint64_t knights, uint64_t unoccupied) {
    //only checks for legal postitions by valid move directions, empty squares & borders, doesn't check checks
    uint64_t h1 = (knights << 8 ) | (knights >> 8 );
    uint64_t h2 = (knights << 16) | (knights >> 16);
    uint64_t knightMoves =  (h1<<2) & (0xFCFCFCFCFCFCFCFC) |
                            (h1>>2) & (0x3F3F3F3F3F3F3F3F) |
                            (h2<<1) & (0xFEFEFEFEFEFEFEFE) |
                            (h2>>1) & (0x7F7F7F7F7F7F7F7F) ;
    return knightMoves & unoccupied;
}

static uint64_t generatePawnPushesWhite(uint64_t pawns, uint64_t empty) {
    //only checks for legal postitions by valid move directions, empty squares & borders, doesn't check checks
    uint64_t singlePushes = northOne(pawns) & empty;
    uint64_t doublePushes = northOne(singlePushes) & empty & 0x0808080808080808;
    return singlePushes | doublePushes;
}
static uint64_t generatePawnPushesBlack(uint64_t pawns, uint64_t empty) {
    //only checks for legal postitions by valid move directions, empty squares & borders, doesn't check checks
    uint64_t singlePushes = southOne(pawns) & empty;
    uint64_t doublePushes = southOne(singlePushes) & empty & 0x1010101010101010;
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
