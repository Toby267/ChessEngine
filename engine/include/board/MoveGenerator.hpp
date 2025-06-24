#pragma once

#include <vector>

#include "board/Board.hpp"
#include "board/Move.hpp"

//TODO: remember to make const again
std::vector<Move> generateMoves(Board& board, bool whiteTurn);

void bitScan();




/**
 * Returns the possible knight moves given the knight position bitboard and all unoccupied squares
 * 
 * @param knightBitBoard the bitboard containing only the knight
 * @param unoccupied a bitboard mask filtering out squares occupied by the same colour
 */
/**
 * Returns the possible king moves given the kings position bitboard and all unoccupied squares
 * 
 * @param king the bitboard containing only the king
 * @param unoccupied a bitboard mask filtering out squares occupied by the same colour
 */
