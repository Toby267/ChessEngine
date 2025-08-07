#pragma once

#include <cstdint>
#include <string>

/**
 * Contains utility typedefs, enums, and functions used to help in various operations on the board such as move generation
*/

typedef bool WhiteTurn;

enum SquareIndex {
    a1, a2, a3, a4, a5, a6, a7, a8,
    b1, b2, b3, b4, b5, b6, b7, b8,
    c1, c2, c3, c4, c5, c6, c7, c8,
    d1, d2, d3, d4, d5, d6, d7, d8,
    e1, e2, e3, e4, e5, e6, e7, e8,
    f1, f2, f3, f4, f5, f6, f7, f8,
    g1, g2, g3, g4, g5, g6, g7, g8,
    h1, h2, h3, h4, h5, h6, h7, h8
};

namespace PieceType {
    #define WHITE  0
    #define BLACK  1
    #define PIECE_COLOUR(p) ((p)&1)

    const std::string pieceChars[] = {"o", "♟", "♙", "♞", "♘", "♝", "♗", "♜", "♖", "♛", "♕", "♚", "♔"};
    const std::string enumOrder = "PpNnBbRrQqKk";

    enum Enum {
        WHITE_PAWN,
        BLACK_PAWN,
        WHITE_KNIGHT,
        BLACK_KNIGHT,
        WHITE_BISHOP,
        BLACK_BISHOP,
        WHITE_ROOK,
        BLACK_ROOK,
        WHITE_QUEEN,
        BLACK_QUEEN,
        WHITE_KING,
        BLACK_KING,
        
        WHITE_PIECES,
        BLACK_PIECES,

        INVALID = -1
    };
}

enum EnPassantPieces {
    A_WHITE, B_WHITE, C_WHITE, D_WHITE, E_WHITE, F_WHITE, G_WHITE, H_WHITE,
    A_BLACK, B_BLACK, C_BLACK, D_BLACK, E_BLACK, F_BLACK, G_BLACK, H_BLACK
};

enum CastlePieces{
    W_KING, W_QUEEN, B_KING, B_QUEEN
};

// * ------------------------------------ [ ORTHOGANOL SHIFTERS INT ] ------------------------------------ * //

//these functions move one piece one unit in the given bitboard in a certain direction
SquareIndex northOne(int piece);
SquareIndex southOne(int piece);
SquareIndex eastOne(int piece);
SquareIndex westOne(int piece);

SquareIndex northEastOne(int piece);
SquareIndex northWestOne(int piece);
SquareIndex southEastOne(int piece);
SquareIndex southWestOne(int piece);

// * ----------------------------------- [ ORTHOGANOL SHIFTERS UINT64 ] ---------------------------------- * //

//these functions move each piece one unit in the given bitboard in a certain direction
uint64_t northOne(uint64_t board);
uint64_t southOne(uint64_t board);
uint64_t eastOne(uint64_t board);
uint64_t westOne(uint64_t board);

uint64_t northEastOne(uint64_t board);
uint64_t northWestOne(uint64_t board);
uint64_t southEastOne(uint64_t board);
uint64_t southWestOne(uint64_t board);

// * ------------------------------------ [ DIRECTION RAY FUNCTIONS ] ------------------------------------ * //

//these functions return a mask for each square in a certain direction from the given square, not including the given square
uint64_t calcNorthMask(SquareIndex square);
uint64_t calcEastMask(SquareIndex square);
uint64_t calcSouthMask(SquareIndex square);
uint64_t calcWestMask(SquareIndex square);

uint64_t calcNorthEastMask(SquareIndex square);
uint64_t calcNorthWestMask(SquareIndex square);
uint64_t calcSouthEastMask(SquareIndex square);
uint64_t calcSouthWestMask(SquareIndex square);
