#include "board/MoveGeneration/TargetBitboardGenerator.hpp"

#include <array>
#include <cstdint>

#include "board/BoardUtil.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ STATIC MEMBERS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

static uint64_t (*const rayFunctions[])(SquareIndex) = {calcNorthMask, calcEastMask, calcSouthMask, calcWestMask, calcNorthEastMask, calcNorthWestMask, calcSouthEastMask, calcSouthWestMask};

static uint64_t generateRookBitboardSingular(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces);
static uint64_t generateBishopBitboardSingular(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces);
static uint64_t generateQueenBitboardSingular(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces);

static uint64_t getPositiveRay(SquareIndex square, uint64_t occupied, Direction dir);
static uint64_t getNegativeRay(SquareIndex square, uint64_t occupied, Direction dir);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ PUBLIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// * ---------------------------------------- [ EASY MOVES ] ---------------------------------------- * //

uint64_t generateKingBitboard(uint64_t king, uint64_t friendlyPieces) {
    //doesn't check checks
    uint64_t kingMoves = king;
    kingMoves |= eastOne(kingMoves)  | westOne(kingMoves);
    kingMoves |= northOne(kingMoves) | southOne(kingMoves);
    return kingMoves & ~friendlyPieces;
}
uint64_t generateKnightBitboard(uint64_t knights, uint64_t friendlyPieces) {
    //doesn't check checks
    uint64_t h1 = (knights << 8 ) | (knights >> 8 );
    uint64_t h2 = (knights << 16) | (knights >> 16);
    uint64_t knightMoves =  (h1<<2) & (0xFCFCFCFCFCFCFCFCULL) |
                            (h1>>2) & (0x3F3F3F3F3F3F3F3FULL) |
                            (h2<<1) & (0xFEFEFEFEFEFEFEFEULL) |
                            (h2>>1) & (0x7F7F7F7F7F7F7F7FULL) ;
    return knightMoves & ~friendlyPieces;
}

// * ---------------------------------------- [ PAWN MOVES ] ---------------------------------------- * //

uint64_t generatePawnPushBitboardWhite(uint64_t pawns, uint64_t unoccupied) {
    //doesn't check checks
    uint64_t singlePushes = northOne(pawns) & unoccupied;
    uint64_t doublePushes = northOne(singlePushes) & unoccupied & 0x0808080808080808ULL;
    return singlePushes | doublePushes;
}
uint64_t generatePawnPushBitboardBlack(uint64_t pawns, uint64_t unoccupied) {
    //doesn't check checks
    uint64_t singlePushes = southOne(pawns) & unoccupied;
    uint64_t doublePushes = southOne(singlePushes) & unoccupied & 0x1010101010101010ULL;
    return singlePushes | doublePushes;
}

uint64_t generatePawnAttackBitboardWhite(uint64_t pawns, uint64_t blackPieces) {
    //doesn't check checks
    uint64_t moves = northEastOne(pawns) | northWestOne(pawns);
    return moves & blackPieces;
}
uint64_t generatePawnAttackBitboardBlack(uint64_t pawns, uint64_t whitePieces) {
    //doesn't check checks
    uint64_t moves = southEastOne(pawns) | southWestOne(pawns);
    return moves & whitePieces;
}

// * ---------------------------------------- [ SLIDING MOVES ] ---------------------------------------- * //

uint64_t generateRookBitboard(uint64_t rooks, uint64_t occupied, uint64_t friendlyPieces) {
    uint64_t moves = 0;

    while (rooks) {
        uint64_t index = __builtin_ctzll(rooks);
        moves |= generateRookBitboardSingular((SquareIndex)(index), occupied, friendlyPieces);
        rooks &= rooks-1;
    }

    return moves;
}
uint64_t generateBishopBitboard(uint64_t bishops, uint64_t occupied, uint64_t friendlyPieces) {
    uint64_t moves = 0;

    while (bishops) {
        uint64_t index = __builtin_ctzll(bishops);
        moves |= generateBishopBitboardSingular((SquareIndex)(index), occupied, friendlyPieces);
        bishops &= bishops-1;
    }

    return moves;
}
uint64_t generateQueenBitboard(uint64_t queens, uint64_t occupied, uint64_t friendlyPieces) {
    uint64_t moves = 0;

    while (queens) {
        uint64_t index = __builtin_ctzll(queens);
        moves |= generateQueenBitboardSingular((SquareIndex)(index), occupied, friendlyPieces);
        queens &= queens-1;
    }

    return moves;
}

// * -------------------------------------- [ SPECIAL MOVES ] --------------------------------------- * //

uint64_t generateCastlingBitboardWhite(uint64_t occupied, std::array<__uint128_t, 4> castleData) {
    //doesn't check checks or if a piece is attacking between the end positions & doesn't check checks
    uint64_t moves = 0ULL;

    if (castleData[CastlePieces::W_KING]  == 0 && (occupied & (uint64_t)(0x0001010000000000)) == 0) {
        moves |= 0x0100000000000000;
    }
    if (castleData[CastlePieces::W_QUEEN] == 0 && (occupied & (uint64_t)(0x0000000001010100)) == 0) {
        moves |= 0x0000000000000001;
    }

    return moves;
}
uint64_t generateCastlingBitboardBlack(uint64_t occupied, std::array<__uint128_t, 4> castleData) {
    //doesn't check checks or if a piece is attacking between the end positions & doesn't check checks
    uint64_t moves = 0ULL;

    if (castleData[CastlePieces::B_KING]  == 0 && (occupied & (uint64_t)(0x0080800000000000)) == 0) {
        moves |= 0x8000000000000000;
    }
    if (castleData[CastlePieces::B_QUEEN] == 0 && (occupied & (uint64_t)(0x0000000080808000)) == 0) {
        moves |= 0x0000000000000080;
    }

    return moves;
}

uint64_t generateEnPassantBitboardWhite(uint64_t friendlyPieces, std::array<__uint128_t, 16> enPassantData){ 
    //doesn't check checks
    for (int i = 0; i < enPassantData.size(); i++) {
        if (!(enPassantData[i] & 1)) continue;

        int index = ((i % 8) * 8) + ((i > 7) ? 4 : 3);

        if (((westOne(1ULL << index) & friendlyPieces) != 0) || ((eastOne(1ULL << index) & friendlyPieces) != 0)) {
            return northOne(1ULL << index);
        }
    }

    return 0ULL;
}
uint64_t generateEnPassantBitboardBlack(uint64_t friendlyPieces, std::array<__uint128_t, 16> enPassantData){ 
    //doesn't check checks
    for (int i = 0; i < enPassantData.size(); i++) {
        if (!(enPassantData[i] & 1)) continue;

        int index = ((i % 8) * 8) + ((i > 7) ? 4 : 3);

        if ((westOne(1ULL << index) & friendlyPieces) != 0 || (eastOne(1ULL << index) & friendlyPieces) != 0) {
            return southOne(1ULL << index);
        }
    }

    return 0ULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ STATIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

static uint64_t generateRookBitboardSingular(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces) {
    //doesn't check checks
    uint64_t moves =    getPositiveRay(square, occupied, Direction::NORTH)  |
                        getPositiveRay(square, occupied, Direction::EAST)   |
                        getNegativeRay(square, occupied, Direction::SOUTH)  |
                        getNegativeRay(square, occupied, Direction::WEST)   ;

    return moves & ~friendlyPieces;
}
static uint64_t generateBishopBitboardSingular(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces) {
    //doesn't check checks
    uint64_t moves =    getNegativeRay(square, occupied, Direction::SOUTH_WEST) |
                        getPositiveRay(square, occupied, Direction::NORTH_EAST) |
                        getPositiveRay(square, occupied, Direction::SOUTH_EAST) |
                        getNegativeRay(square, occupied, Direction::NORTH_WEST) ;

    return moves & ~friendlyPieces;
}
static uint64_t generateQueenBitboardSingular(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces) {
    //doesn't check checks
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