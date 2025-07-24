#pragma once

#include <vector>

#include "board/Move.hpp"
#include "board/Board.hpp"
#include "board/BoardUtil.hpp"

std::vector<Move> generateKingMoves(WhiteTurn whiteTurn, uint64_t king, uint64_t friendlyPieces);
std::vector<Move> generateKnightMoves(WhiteTurn whiteTurn, uint64_t knights, uint64_t friendlyPieces);

std::vector<Move> generatePawnMoves(WhiteTurn whiteTurn, uint64_t pawns, uint64_t unoccupied);

std::vector<Move> generateRookMoves(WhiteTurn whiteTurn, uint64_t rooks, uint64_t occupied, uint64_t friendlyPieces);
std::vector<Move> generateBishopMoves(WhiteTurn whiteTurn, uint64_t bishops, uint64_t occupied, uint64_t friendlyPieces);
std::vector<Move> generateQueenMoves(WhiteTurn whiteTurn, uint64_t queens, uint64_t occupied, uint64_t friendlyPieces);

std::vector<Move> generateCastlingMoves(WhiteTurn whiteTurn, const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData);
std::vector<Move> generateEnPassantMoves(WhiteTurn whiteTurn, uint64_t friendlyPieces, std::array<__uint128_t, 16> enPassantData);
