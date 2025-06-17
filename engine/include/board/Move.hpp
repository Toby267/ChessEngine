#pragma once

#include <board/Board.hpp>

enum MoveType {
    NORMAL,
    PROMOTION,
    EN_PASSANT,
    CASTLE
};

struct NormalMove {
    MoveType flag;
    SquareIndex startPos;
    SquareIndex endPos;
    PieceType pieceType;
};

struct PromotionMove {
    MoveType flag;
    SquareIndex startPos;
    SquareIndex endPos;
    PieceType newPieceType;
};

struct EnPassantMove {
    MoveType flag;
    SquareIndex startPos;
    SquareIndex endPos;
    SquareIndex killSquare;
};

struct CastleMove {
    MoveType flag;
    SquareIndex primaryStartPos;
    SquareIndex primaryEndPos;
    SquareIndex secondaryStartPos;
    SquareIndex secondaryEndPos;
};

union Move {
    NormalMove normalMove;
    PromotionMove promotionMove;
    EnPassantMove enPassantMove;
    CastleMove castleMove;
};

// struct Move {
//     SquareIndex primaryStartPos;
//     SquareIndex primaryEndPos;
//     SquareIndex secondaryStartPos;
//     SquareIndex secondaryEndPos;
//     SquareIndex killSquare;
//     PieceType oldPieceType;
//     PieceType newPieceType;
// };
