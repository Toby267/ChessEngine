#pragma once

#include <vector>

#include "board/Move.hpp"
#include "board/Board.hpp"
#include "board/BoardUtil.hpp"

std::vector<Move> generateKingMoves(Colour colourToPlay, uint64_t king, uint64_t friendlyPieces);
std::vector<Move> generateKnightMoves(Colour colourToPlay, uint64_t knights, uint64_t friendlyPieces);

std::vector<Move> generatePawnMoves(Colour colourToPlay, uint64_t pawns, uint64_t unoccupied);

std::vector<Move> generateRookMoves(Colour colourToPlay, uint64_t rooks, uint64_t occupied, uint64_t friendlyPieces);
std::vector<Move> generateBishopMoves(Colour colourToPlay, uint64_t bishops, uint64_t occupied, uint64_t friendlyPieces);
std::vector<Move> generateQueenMoves(Colour colourToPlay, uint64_t queens, uint64_t occupied, uint64_t friendlyPieces);

std::vector<Move> generateCastlingMoves(Colour colourToPlay, const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData);

std::vector<Move> generateEnPassantMoves(Colour colourToPlay, uint64_t friendlyPieces, std::array<__uint128_t, 16> enPassantData);
