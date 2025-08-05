#pragma once

#include <vector>

#include "board/Board.hpp"
#include "board/BoardUtil.hpp"

/**
 * Contains various functions used for generating target bitboards, and piece moves for a given board
 * 
 * Used by the move generator function to generate all possible moves for a given board
*/

// * ------------------------------------------ [ IS TARGETED ] ------------------------------------------ * //

bool isTargeted(const Board& board, WhiteTurn whiteTurn, SquareIndex i);

// * ----------------------------------- [ BITBOARD MOVE SERIALISATION ] --------------------------------- * //

void generateKingMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t king, uint64_t friendlyPieces);
void generateKnightMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t knights, uint64_t friendlyPieces);

void generateRookMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t rooks, uint64_t occupied, uint64_t friendlyPieces);
void generateBishopMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t bishops, uint64_t occupied, uint64_t friendlyPieces);
void generateQueenMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t queens, uint64_t occupied, uint64_t friendlyPieces);

void generatePawnMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t pawns, uint64_t unoccupied, uint64_t oppositionPieces);
void generateCastlingMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t occupied, std::array<__uint128_t, 4> castleData);
void generateEnPassantMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t pawns, std::array<__uint128_t, 16> enPassantData);

// * ------------------------------------ [ BITBOARD MOVE GENERATION ] ----------------------------------- * //

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
uint64_t generatePawnTargetBitboard(WhiteTurn whiteTurn, uint64_t pawns);
uint64_t generateEnPassantBitboard(WhiteTurn whiteTurn, uint64_t pawns, std::array<__uint128_t, 16> enPassantData);