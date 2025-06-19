#pragma once

#include <cstdint>
#include <string>
#include <optional>

#include <board/BoardEnums.hpp>
#include <board/Move.hpp>

/*
    Stores all 14 bitboards, indexed by the PieceType enum, using Little-Endian File-Rank Mapping/
    Little-Endian Least-Significant-Rank Mapping as shown in the SquareIndex enum.
*/
class Board {
private:
    uint64_t bitBoards[14];
    bool whiteCastleKing, whiteCastleQueen, blackCastleKing, blackCastleQueen;
    std::optional<SquareIndex> enPassantSquare;

public:
    //constructors/destructor
    Board();
    ~Board();

    //getters/setters
    PieceType getType(SquareIndex index);

    //public methods
    void makeMove(const Move& move);
    void unMakeMove(const Move& move);

    void setDefaultBoard();
    void resetBoard();

    void parseFen(const std::string& FEN);

    void printDebugData();

private:
    //private methods
    void addPiece(PieceType type, SquareIndex index);
    void removePiece(PieceType type, SquareIndex index);
    void togglePiece(PieceType type, SquareIndex index);    

    void printBitBoard(PieceType board);
    void printBitBoardHex(PieceType board);

    //helper methods
    bool isWhite(SquareIndex index);
    bool isBlack(SquareIndex index);
    bool hasPiece(SquareIndex index);
};
