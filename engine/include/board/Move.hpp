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
    SquareIndex         startPos;
    SquareIndex         endPos;
    PieceType::Enum     pieceType;
    PieceType::Enum     killPieceType;
};

struct PromotionMove {
    SquareIndex         startPos;
    SquareIndex         endPos;
    PieceType::Enum     oldPieceType;
    PieceType::Enum     newPieceType;
    PieceType::Enum     killPieceType;
};

struct EnPassantMove {
    SquareIndex         startPos;
    SquareIndex         endPos;
    PieceType::Enum     pieceType;
    SquareIndex         killSquare;
    PieceType::Enum     killPieceType;
};

struct CastleMove {
    SquareIndex         primaryStartPos;
    SquareIndex         primaryEndPos;
    PieceType::Enum     primaryPieceType;   //white king or black king
    SquareIndex         secondaryStartPos;
    SquareIndex         secondaryEndPos;
    PieceType::Enum     secondaryPieceType; //white rook or black rook
};

struct Move {
    MoveType flag;
    union {
        NormalMove    normalMove;
        PromotionMove promotionMove;
        EnPassantMove enPassantMove;
        CastleMove    castleMove;
    };

    //default constructor
    Move() : flag(MoveType(0)) {}

    //constructors for each move type
    Move(MoveType flag, NormalMove move) :
        flag(flag),
        normalMove(move)
    {}

    Move(MoveType flag, PromotionMove move) :
        flag(flag),
        promotionMove(move)
    {}

    Move(MoveType flag, EnPassantMove move) :
        flag(flag),
        enPassantMove(move)
    {}

    Move(MoveType flag, CastleMove move) :
        flag(flag),
        castleMove(move)
    {}
};

void printMove(const Move& move);
