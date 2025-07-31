#pragma once

#include <vector>

#include "board/Move.hpp"
#include "board/Board.hpp"
#include "board/BoardUtil.hpp"

void generateKingMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t king, uint64_t friendlyPieces);
void generateKnightMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t knights, uint64_t friendlyPieces);

void generateRookMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t rooks, uint64_t occupied, uint64_t friendlyPieces);
void generateBishopMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t bishops, uint64_t occupied, uint64_t friendlyPieces);
void generateQueenMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t queens, uint64_t occupied, uint64_t friendlyPieces);

void generatePawnMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t pawns, uint64_t unoccupied, uint64_t oppositionPieces);
void generateCastlingMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t occupied, std::array<__uint128_t, 4> castleData);
void generateEnPassantMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t friendlyPieces, std::array<__uint128_t, 16> enPassantData);
