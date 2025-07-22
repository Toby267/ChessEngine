#pragma once

#include <array>
#include <cstdint>

#include "board/BoardUtil.hpp"

enum Direction {
    NORTH,
    EAST,
    SOUTH,
    WEST,
    NORTH_EAST,
    NORTH_WEST,
    SOUTH_EAST,
    SOUTH_WEST
};

//public methods
uint64_t generateKingBitboard(uint64_t king, uint64_t friendlyPieces);
uint64_t generateKnightBitboard(uint64_t knights, uint64_t friendlyPieces);

uint64_t generatePawnPushBitboardWhite(uint64_t pawns, uint64_t unoccupied);
uint64_t generatePawnPushBitboardBlack(uint64_t pawns, uint64_t unoccupied);
uint64_t generatePawnAttackBitboardWhite(uint64_t pawns, uint64_t blackPieces);
uint64_t generatePawnAttackBitboardBlack(uint64_t pawns, uint64_t whitePieces);

uint64_t generateRookBitboardSingular(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces);
uint64_t generateBishopBitboardSingular(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces);
uint64_t generateQueenBitboardSingular(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces);

uint64_t generateCastlingBitboardWhite(uint64_t occupied, std::array<__uint128_t, 4> castleData);
uint64_t generateCastlingBitboardBlack(uint64_t occupied, std::array<__uint128_t, 4> castleData);
uint64_t generateEnPassantBitboardWhite(uint64_t friendlyPieces, std::array<__uint128_t, 16> enPassantData);
uint64_t generateEnPassantBitboardBlack(uint64_t friendlyPieces, std::array<__uint128_t, 16> enPassantData);
