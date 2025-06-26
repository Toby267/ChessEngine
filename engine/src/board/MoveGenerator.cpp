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

static uint64_t getPositiveRay(SquareIndex square, uint64_t occupied, Direction dir);
static uint64_t getNegativeRay(SquareIndex square, uint64_t occupied, Direction dir);

// * ---------------------------------- [ PUBLIC METHODS ] ----------------------------------- * //

/**
 * Generates all possible moves based on a given board and whos to move
 * 
 * @param board the board
 * @param whiteTurn whether or not it is whites turn to move
 */
std::vector<Move> generateMoves(Board& board, bool whiteTurn, SquareIndex temp) {//todo: remove the temp variable
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
    //still need to test pawn moves
    uint64_t pawnPushes     = whiteTurn ?   generatePawnPushesWhite(bitBoards[PieceType::WHITE_PAWN], unoccupied):
                                            generatePawnPushesBlack(bitBoards[PieceType::BLACK_PAWN], unoccupied);
    uint64_t pawnAttacks    = whiteTurn ?   generatePawnAttacksWhite(bitBoards[PieceType::WHITE_PAWN], blackPieces):
                                            generatePawnAttacksBlack(bitBoards[PieceType::BLACK_PAWN], whitePieces);

    //generate moves for sliding pieces
    //need to extract rook, bishop, & queen positions before putting them into this
    //need to also finish the functinality with the unoccupied mask
    uint64_t rookMoves      = generateRookMoves(temp, (uint64_t)(0));
    //uint64_t bishopMoves    = generateBishopMoves(temp, (uint64_t)(0));
    //uint64_t queenMoves     = generateQueenMoves(temp, (uint64_t)(0));

    //generate en passant and castling

    board.setBitBoard(rookMoves, PieceType::BLACK_KING);
    board.setBitBoard(rookMoves, PieceType::BLACK_PIECES);

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



static uint64_t generatePawnPushesWhite(uint64_t pawns, uint64_t unoccupied) {
    //only checks for legal postitions by valid move directions, empty squares & borders, doesn't check checks
    uint64_t singlePushes = northOne(pawns) & unoccupied;
    uint64_t doublePushes = northOne(singlePushes) & unoccupied & 0x0808080808080808;
    return singlePushes | doublePushes;
}

static uint64_t generatePawnPushesBlack(uint64_t pawns, uint64_t unoccupied) {
    //only checks for legal postitions by valid move directions, empty squares & borders, doesn't check checks
    uint64_t singlePushes = southOne(pawns) & unoccupied;
    uint64_t doublePushes = southOne(singlePushes) & unoccupied & 0x1010101010101010;
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



static uint64_t generateRookMoves(SquareIndex square, uint64_t occupied) {
    //TODO: finish this - need to use the mask in someway - probably manipulate it someway before ORing together
    //uint64_t mask = calcNorthMask(square) | calcEastMask(square) | calcSouthMask(square) | calcWestMask(square);
    //return mask;

    // return  getPositiveRay(square, occupied, Direction::NORTH)  |
    //         getPositiveRay(square, occupied, Direction::EAST)   |
    //         getNegativeRay(square, occupied, Direction::SOUTH)  |
    //         getNegativeRay(square, occupied, Direction::WEST)   ;

    //return getPositiveRay(square, occupied, Direction::NORTH);//yes
    //return getPositiveRay(square, occupied, Direction::EAST);//no
    //return getPositiveRay(square, occupied, Direction::NORTH_EAST);//yes
    //return getPositiveRay(square, occupied, Direction::SOUTH_EAST);//no

    //return getNegativeRay(square, occupied, Direction::SOUTH);//yes
    //return getNegativeRay(square, occupied, Direction::WEST);//no
    //return getPositiveRay(square, occupied, Direction::NORTH_WEST);//yes
    //return getPositiveRay(square, occupied, Direction::SOUTH_WEST);//no
}

static uint64_t generateBishopMoves(SquareIndex square, uint64_t occupied) {
    //TODO: finish this - need to use the mask in someway - probably manipulate it someway before ORing together
    //uint64_t mask = calcNorthEastMask(square) | calcSouthEastMask(square) | calcSouthWestMask(square) | calcNorthWestMask(square);
    //return mask;

    return  getNegativeRay(square, occupied, Direction::SOUTH_WEST) |
            getPositiveRay(square, occupied, Direction::NORTH_EAST) |
            getPositiveRay(square, occupied, Direction::SOUTH_EAST) |
            getNegativeRay(square, occupied, Direction::NORTH_WEST) ;
            
}

static uint64_t generateQueenMoves(SquareIndex square, uint64_t occupied) {
    //TODO: finish this - need to use the mask in someway - probably manipulate it someway before ORing together
    //uint64_t mask = generateRookMoves(square, occupied) | generateBishopMoves(square, occupied);
    //return mask;

    return  getPositiveRay(square, occupied, Direction::NORTH)      |
            getPositiveRay(square, occupied, Direction::EAST)       |
            getNegativeRay(square, occupied, Direction::SOUTH)      |
            getNegativeRay(square, occupied, Direction::WEST)       |
            getNegativeRay(square, occupied, Direction::SOUTH_WEST) |
            getPositiveRay(square, occupied, Direction::NORTH_EAST) |
            getPositiveRay(square, occupied, Direction::SOUTH_EAST) |
            getNegativeRay(square, occupied, Direction::NORTH_WEST) ;
}

// * ---------------------------------- [ HELPER METHODS ] ---------------------------------- * //

//one or both of the following are broke in some way

static uint64_t getPositiveRay(SquareIndex square, uint64_t occupied, Direction dir) {
    uint64_t ray = rayFunctions[dir](square);
    uint64_t blockers = ray & occupied;
    int firstBlocker = __builtin_clz(blockers);
    ray ^= rayFunctions[dir]((SquareIndex)firstBlocker);
    return ray;
}

static uint64_t getNegativeRay(SquareIndex square, uint64_t occupied, Direction dir) {
    uint64_t ray = rayFunctions[dir](square);
    uint64_t blockers = ray & occupied;
    int firstBlocker = __builtin_ctz(blockers);
    ray ^= rayFunctions[dir]((SquareIndex)firstBlocker);
    return ray;
}
