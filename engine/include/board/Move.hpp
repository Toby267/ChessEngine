#pragma once

#include "BoardUtil.hpp"

/**
 * Contains the structs, enums and functions used to store and manipulate any possible chess move
 * 
 * Must keep the order for startPos, endPos and pieceType the same for NormalMove, PromotionMove, and CastleMove
 * for the genericupdateSpecialMoveStatus() method in Board.cpp
 *
 * If you change their order in any way, use the MoveType specific methods in commit: b316dec916c7e2c140d3900867e1b1c7139a67c1
*/

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

void printMove(const Move& move);
