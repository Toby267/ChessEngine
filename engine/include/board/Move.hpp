#pragma once

#include <board/BoardEnums.hpp>

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
    PieceType oldPieceType;
    PieceType newPieceType;
};

struct EnPassantMove {
    MoveType flag;
    SquareIndex startPos;
    SquareIndex endPos;
    PieceType pieceType;
    SquareIndex killSquare;
    PieceType killPieceType;
};

struct CastleMove {
    MoveType flag;
    SquareIndex primaryStartPos;
    SquareIndex primaryEndPos;
    PieceType primaryPieceType;
    SquareIndex secondaryStartPos;
    SquareIndex secondaryEndPos;
    PieceType secondaryPieceType;
};

union Move {
    NormalMove normalMove;
    PromotionMove promotionMove;
    EnPassantMove enPassantMove;
    CastleMove castleMove;
};
