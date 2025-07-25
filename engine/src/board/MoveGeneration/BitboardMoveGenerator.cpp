#include "board/MoveGeneration/BitboardMoveGenerator.hpp"

#include <array>
#include <cstdint>

#include "board/BoardUtil.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ STATIC MEMBERS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

static uint64_t (*const rayFunctions[])(SquareIndex) = {calcNorthMask, calcEastMask, calcSouthMask, calcWestMask, calcNorthEastMask, calcNorthWestMask, calcSouthEastMask, calcSouthWestMask};

static uint64_t getPositiveRay(SquareIndex square, uint64_t occupied, Direction dir);
static uint64_t getNegativeRay(SquareIndex square, uint64_t occupied, Direction dir);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ PUBLIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Returns if a specific square is being targeted by any pseduo legal move in a given position 
 * 
 * @param board the board
 * @param whiteTurn whether or not it is whites turn to move
 * @param i the given square index
 */
bool isTargeted(const Board& board, WhiteTurn whiteTurn, SquareIndex i) {
    //targeted piece
    uint64_t targetedPiece = 1ULL << i;
    
    //constant values including the bitboards and masks
    const std::array<uint64_t, 14>&     bitBoards           = board.getBitBoards();
    const std::array<__uint128_t, 16>&  enPassantData       = board.getEnPassantData();
    const uint64_t                      whitePieces         = bitBoards[PieceType::WHITE_PIECES];
    const uint64_t                      blackPieces         = bitBoards[PieceType::BLACK_PIECES];
    const uint64_t                      friendlyPieces      = whiteTurn ? whitePieces : blackPieces;
    const uint64_t                      oppositionPieces    = whiteTurn ? blackPieces : whitePieces;
    const uint64_t                      occupied            = whitePieces | blackPieces;
    const short                         indexOffset         = whiteTurn ? 0 : 6;

    //generate bitboards for the king and knights
    uint64_t kingMoves      = generateKingBitboard(bitBoards[PieceType::WHITE_KING + indexOffset], friendlyPieces);
    uint64_t knightMoves    = generateKnightBitboard(bitBoards[PieceType::WHITE_KNIGHT + indexOffset], friendlyPieces);
    if (targetedPiece & kingMoves || targetedPiece & knightMoves) return  true;

    //generate bitboards for sliding pieces
    uint64_t rookMoves      = generateRookBitboard(bitBoards[PieceType::WHITE_ROOK + indexOffset], occupied, friendlyPieces);
    uint64_t bishopMoves    = generateBishopBitboard(bitBoards[PieceType::WHITE_BISHOP + indexOffset], occupied, friendlyPieces);
    uint64_t queenMoves     = generateQueenBitboard(bitBoards[PieceType::WHITE_QUEEN + indexOffset], occupied, friendlyPieces);
    if (targetedPiece & rookMoves || targetedPiece & bishopMoves || targetedPiece & queenMoves) return  true;

    //generate bitboards for pawns
    uint64_t pawnAttacks    = generatePawnAttackBitboard(whiteTurn, bitBoards[PieceType::WHITE_PAWN + indexOffset], oppositionPieces);
    uint64_t enPassantMoves = generateEnPassantBitboard(whiteTurn, bitBoards[PieceType::WHITE_PAWN + indexOffset], enPassantData);
    if (targetedPiece & pawnAttacks || targetedPiece & enPassantMoves) return  true;

    return false;
}

// * ------------------------------------------- [ KING MOVES ] ------------------------------------------ * //

//generates a bitboard of all king target squares
uint64_t generateKingBitboard(uint64_t king, uint64_t friendlyPieces) {
    uint64_t kingMoves = king;
    kingMoves |= eastOne(kingMoves)  | westOne(kingMoves);
    kingMoves |= northOne(kingMoves) | southOne(kingMoves);
    return kingMoves & ~friendlyPieces;
}

// * ------------------------------------------ [ KNIGHT MOVES ] ----------------------------------------- * //

//generates a bitboard of all knight target squares for all knights
uint64_t generateKnightBitboard(uint64_t knights, uint64_t friendlyPieces) {
    uint64_t h1 = (knights << 8 ) | (knights >> 8 );
    uint64_t h2 = (knights << 16) | (knights >> 16);
    uint64_t knightMoves =  (h1<<2) & (0xFCFCFCFCFCFCFCFCULL) |
                            (h1>>2) & (0x3F3F3F3F3F3F3F3FULL) |
                            (h2<<1) & (0xFEFEFEFEFEFEFEFEULL) |
                            (h2>>1) & (0x7F7F7F7F7F7F7F7FULL) ;
    return knightMoves & ~friendlyPieces;
}
//generates a bitboard of all knight target squares
uint64_t generateKnightBitboardSingular(SquareIndex square, uint64_t friendlyPieces) {
    return generateKnightBitboard(1ULL << square, friendlyPieces);
}

// * ------------------------------------------- [ ROOK MOVES ] ------------------------------------------ * //

//generates a bitboard of all rook target squares for all rooks
uint64_t generateRookBitboard(uint64_t rooks, uint64_t occupied, uint64_t friendlyPieces) {
    uint64_t moves = 0;

    while (rooks) {
        uint64_t index = __builtin_ctzll(rooks);
        moves |= generateRookBitboardSingular((SquareIndex)(index), occupied, friendlyPieces);
        rooks &= rooks-1;
    }

    return moves;
}
//generates a bitboard of all rook target squares
uint64_t generateRookBitboardSingular(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces) {
    uint64_t moves =    getPositiveRay(square, occupied, Direction::NORTH)  |
                        getPositiveRay(square, occupied, Direction::EAST)   |
                        getNegativeRay(square, occupied, Direction::SOUTH)  |
                        getNegativeRay(square, occupied, Direction::WEST)   ;

    return moves & ~friendlyPieces;
}

// * ------------------------------------------ [ BISHOP MOVES ] ----------------------------------------- * //

//generates a bitboard of all bishop target squares for all bishops
uint64_t generateBishopBitboard(uint64_t bishops, uint64_t occupied, uint64_t friendlyPieces) {
    uint64_t moves = 0;

    while (bishops) {
        uint64_t index = __builtin_ctzll(bishops);
        moves |= generateBishopBitboardSingular((SquareIndex)(index), occupied, friendlyPieces);
        bishops &= bishops-1;
    }

    return moves;
}
//generates a bitboard of all bishop target squares
uint64_t generateBishopBitboardSingular(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces) {
    uint64_t moves =    getNegativeRay(square, occupied, Direction::SOUTH_WEST) |
                        getPositiveRay(square, occupied, Direction::NORTH_EAST) |
                        getPositiveRay(square, occupied, Direction::SOUTH_EAST) |
                        getNegativeRay(square, occupied, Direction::NORTH_WEST) ;

    return moves & ~friendlyPieces;
}

// * ------------------------------------------ [ QUEEN MOVES ] ------------------------------------------ * //

//generates a bitboard of all queen target squares for all queens
uint64_t generateQueenBitboard(uint64_t queens, uint64_t occupied, uint64_t friendlyPieces) {
    uint64_t moves = 0;

    while (queens) {
        uint64_t index = __builtin_ctzll(queens);
        moves |= generateQueenBitboardSingular((SquareIndex)(index), occupied, friendlyPieces);
        queens &= queens-1;
    }

    return moves;
}
//generates a bitboard of all queen target squares
uint64_t generateQueenBitboardSingular(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces) {
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

// * ------------------------------------------- [ PAWN MOVES ] ------------------------------------------ * //

//generates a bitboard of all pawn push target squares for all pawns
uint64_t generatePawnPushBitboard(WhiteTurn whiteTurn, uint64_t pawns, uint64_t unoccupied) {
    if (whiteTurn) {
        uint64_t singlePushes = northOne(pawns) & unoccupied;
        uint64_t doublePushes = northOne(singlePushes) & unoccupied & 0x0808080808080808ULL;
        return singlePushes | doublePushes;
    }
    else {
        uint64_t singlePushes = southOne(pawns) & unoccupied;
        uint64_t doublePushes = southOne(singlePushes) & unoccupied & 0x1010101010101010ULL;
        return singlePushes | doublePushes;
    }   
}
//generates a bitboard of all pawn attack target squares for all pawns
uint64_t generatePawnAttackBitboard(WhiteTurn whiteTurn, uint64_t pawns, uint64_t oppositionPieces) {
    if (whiteTurn) {
        uint64_t moves = northEastOne(pawns) | northWestOne(pawns);
        return moves & oppositionPieces;
    }
    else {
        uint64_t moves = southEastOne(pawns) | southWestOne(pawns);
        return moves & oppositionPieces;
    }   
}
//generates a bitboard of the en passant target square if there is one
uint64_t generateEnPassantBitboard(WhiteTurn whiteTurn, uint64_t friendlyPieces, std::array<__uint128_t, 16> enPassantData){ 
    for (int i = 0; i < enPassantData.size(); i++) {
        if (!(enPassantData[i] & 1)) continue;

        int index = ((i % 8) * 8) + ((i > 7) ? 4 : 3);
        uint64_t pawnBitboard = 1ULL << index;

        if (pawnBitboard & friendlyPieces) continue;

        if ((westOne(pawnBitboard) & friendlyPieces) || (eastOne(pawnBitboard) & friendlyPieces)) {
            return whiteTurn ? northOne(pawnBitboard) : southOne(pawnBitboard);
        }
    }

    return 0ULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ STATIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Returns a bitboard of target squares from a given square in a given positive/negative direction taking into account occupied squares
 * Assumes you can still take occupied squares
 */
static uint64_t getPositiveRay(SquareIndex square, uint64_t occupied, Direction dir) {
    uint64_t ray = rayFunctions[dir](square);
    uint64_t blockers = ray & occupied;
    int firstBlocker = __builtin_ctzll(blockers | 0x8000000000000000ULL);
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