#pragma once

#include <cstdint>

#include <board/BoardEnums.hpp>
#include <board/Move.hpp>

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
    PieceType getType(SquareIndex index);

    //public methods
    void makeMove(const Move& move);
    void unMakeMove(const Move& move);

    void setupBoard();

    void printBitBoard(PieceType board);
    void printBitBoardHex(PieceType board);

private:
    //private methods
    void addPiece(PieceType type, SquareIndex index);
    void removePiece(PieceType type, SquareIndex index);
    void togglePiece(PieceType type, SquareIndex index);

    void setupDefaultBoard();

    //helper methods
    bool isWhite(SquareIndex index);
    bool isBlack(SquareIndex index);
    bool hasPiece(SquareIndex index);
};
