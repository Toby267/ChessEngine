#pragma once

#include <cstdint>

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

enum PieceType {
    WHITE_KING,
    WHITE_QUEEN,
    WHITE_BISHOP,
    WHITE_KNIGHT,
    WHITE_ROOK,
    WHITE_PAWN,

    BLACK_KING,
    BLACK_QUEEN,
    BLACK_BISHOP,
    BLACK_KNIGHT,
    BLACK_ROOK,
    BLACK_PAWN,

    WHITE_PIECES,
    BLACK_PIECES,

    INVALID = -1
};

uint64_t northOne(uint64_t b);
uint64_t southOne(uint64_t b);
uint64_t eastOne(uint64_t b);
uint64_t westOne(uint64_t b);

uint64_t northEastOne(uint64_t b);
uint64_t northWestOne(uint64_t b);
uint64_t southEastOne(uint64_t b);
uint64_t southWestOne(uint64_t b);
