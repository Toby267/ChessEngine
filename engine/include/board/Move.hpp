#pragma once

#include <board/BoardEnums.hpp>

enum MoveType {
    NORMAL,
    PROMOTION,
    EN_PASSANT,
    CASTLE
};

struct NormalMove {
    SquareIndex startPos;
    SquareIndex endPos;
    PieceType pieceType;
    PieceType killPieceType;
};

struct PromotionMove {
    SquareIndex startPos;
    SquareIndex endPos;
    PieceType oldPieceType;
    PieceType newPieceType;
    PieceType killPieceType;
};

struct EnPassantMove {
    SquareIndex startPos;
    SquareIndex endPos;
    PieceType pieceType;
    SquareIndex killSquare;
    PieceType killPieceType;
};

struct CastleMove {
    SquareIndex primaryStartPos;
    SquareIndex primaryEndPos;
    PieceType primaryPieceType;
    SquareIndex secondaryStartPos;
    SquareIndex secondaryEndPos;
    PieceType secondaryPieceType;
};

struct Move {
    MoveType flag;
    union {
        NormalMove normalMove;
        PromotionMove promotionMove;
        EnPassantMove enPassantMove;
        CastleMove castleMove;
    };
};
