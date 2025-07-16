#pragma once

#include <board/BoardUtil.hpp>

enum MoveType {
    NORMAL,
    PROMOTION,
    EN_PASSANT,
    CASTLE
};

struct NormalMove {
    SquareIndex startPos;
    SquareIndex endPos;
    PieceType   pieceType;
    PieceType   killPieceType;
};

struct PromotionMove {
    SquareIndex startPos;
    SquareIndex endPos;
    PieceType   oldPieceType;
    PieceType   newPieceType;
    PieceType   killPieceType;
};

struct EnPassantMove {
    SquareIndex startPos;
    SquareIndex endPos;
    PieceType   pieceType;
    SquareIndex killSquare;
    PieceType   killPieceType;
};

struct CastleMove {
    SquareIndex primaryStartPos;
    SquareIndex primaryEndPos;
    PieceType   primaryPieceType;   //white king or black king
    SquareIndex secondaryStartPos;
    SquareIndex secondaryEndPos;
    PieceType   secondaryPieceType; //white rook or black rook
};

struct Move {
    MoveType flag;
    union {
        NormalMove    normalMove;
        PromotionMove promotionMove;
        EnPassantMove enPassantMove;
        CastleMove    castleMove;
    };
};
