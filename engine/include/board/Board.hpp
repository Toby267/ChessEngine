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

    INVALID
};

/*
    Stores all 14 bitboards, indexed by the PieceType enum, using Little-Endian File-Rank Mapping/
    Little-Endian Least-Significant-Rank Mapping as shown in the SquareIndex enum.
*/
class Board {
private:
    uint64_t bitBoards[14];

public:
    //constructors/destructor
    Board();
    ~Board();

    //getters/setters
    PieceType getType(const SquareIndex index);

    //public methods
    void makeMove();
    void unMakeMove();

    void setupBoard();

private:
    //private methods
    void addPiece(const PieceType type, const SquareIndex index);
    void removePiece(const PieceType type, const SquareIndex index);
    void togglePiece(const PieceType type, const SquareIndex index);

    void printBitBoard(const PieceType board);
    void printBitBoardHex(const PieceType board);

    void setupDefaultBoard();

    //helper methods
    bool isWhite(const SquareIndex index);
    bool isBlack(const SquareIndex index);
    bool hasPiece(const SquareIndex index);
};
