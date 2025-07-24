#pragma once

#include <array>
#include <cstdint>

#include "board/Board.hpp"
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

bool isTargeted(const Board& board, WhiteTurn whiteTurn, SquareIndex i);

uint64_t generateKingBitboard(uint64_t king, uint64_t friendlyPieces);

uint64_t generateKnightBitboard(uint64_t knights, uint64_t friendlyPieces);
uint64_t generateKnightBitboardSingular(SquareIndex square, uint64_t friendlyPieces);

uint64_t generateRookBitboard(uint64_t rooks, uint64_t occupied, uint64_t friendlyPieces);
uint64_t generateRookBitboardSingular(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces);

uint64_t generateBishopBitboard(uint64_t bishops, uint64_t occupied, uint64_t friendlyPieces);
uint64_t generateBishopBitboardSingular(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces);

uint64_t generateQueenBitboard(uint64_t queens, uint64_t occupied, uint64_t friendlyPieces);
uint64_t generateQueenBitboardSingular(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces);

uint64_t generatePawnPushBitboard(WhiteTurn whiteTurn, uint64_t pawns, uint64_t unoccupied);
uint64_t generatePawnAttackBitboard(WhiteTurn whiteTurn, uint64_t pawns, uint64_t oppositionPieces);
uint64_t generateEnPassantBitboard(WhiteTurn whiteTurn, uint64_t friendlyPieces, std::array<__uint128_t, 16> enPassantData);
