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

bool isTargeted(const Board& board, bool whiteTurn, SquareIndex i);

//easy piece moves
uint64_t generateKingBitboard(uint64_t king, uint64_t friendlyPieces);
uint64_t generateKnightBitboard(uint64_t knights, uint64_t friendlyPieces);
uint64_t generateKnightBitboardSingular(SquareIndex square, uint64_t friendlyPieces);

//pawn moves
uint64_t generatePawnPushBitboardWhite(uint64_t pawns, uint64_t unoccupied);
uint64_t generatePawnPushBitboardBlack(uint64_t pawns, uint64_t unoccupied);
uint64_t generatePawnAttackBitboardWhite(uint64_t pawns, uint64_t blackPieces);
uint64_t generatePawnAttackBitboardBlack(uint64_t pawns, uint64_t whitePieces);
//uint64_t generatePawnBitboardSingular()? or do i bitscan through the above functions? or do the generateInitialSquares() thing from the wiki


//sliding piece moves
uint64_t generateRookBitboard(uint64_t rooks, uint64_t occupied, uint64_t friendlyPieces);
uint64_t generateRookBitboardSingular(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces);

uint64_t generateBishopBitboard(uint64_t bishops, uint64_t occupied, uint64_t friendlyPieces);
uint64_t generateBishopBitboardSingular(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces);

uint64_t generateQueenBitboard(uint64_t queens, uint64_t occupied, uint64_t friendlyPieces);
uint64_t generateQueenBitboardSingular(SquareIndex square, uint64_t occupied, uint64_t friendlyPieces);

//special moves
uint64_t generateEnPassantBitboardWhite(uint64_t friendlyPieces, std::array<__uint128_t, 16> enPassantData);
uint64_t generateEnPassantBitboardBlack(uint64_t friendlyPieces, std::array<__uint128_t, 16> enPassantData);
