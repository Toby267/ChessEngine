#include "board/Board.hpp"

#include <iostream>
#include <bitset>
#include <string>
#include <inttypes.h>

#include "board/Move.hpp"

// * ---------------------------------- [ CONSTRUCTORS/DESCTUCTOR ] ---------------------------------- * //

Board::Board() {
    setupDefaultBoard();
}

Board::~Board() {
    
}

// * ---------------------------------- [ GETTERS/SETTERS ] ---------------------------------- * //

PieceType Board::getType(SquareIndex index) {
    if (isWhite(index)) {
        for (int i = 0; i < 6; i++) {
            if (bitBoards[i] & (1ULL << index)) {
                return (PieceType)i;
            }
        }
    }
    else if (isBlack(index)) {
        for (int i = 6; i < 12; i++) {
            if (bitBoards[i] & (1ULL << index)) {
                return (PieceType)i;
            }
        }
    }
    
    return PieceType::INVALID;
}

// * ------------------------------------- [ PUBLIC METHODS ] --------------------------------------- * //

void Board::makeMove(const Move& move) {
    switch (move.flag) {
        case MoveType::CASTLE:
            togglePiece(move.castleMove.primaryPieceType, move.castleMove.primaryStartPos);
            togglePiece(move.castleMove.primaryPieceType, move.castleMove.primaryEndPos);
            togglePiece(move.castleMove.secondaryPieceType, move.castleMove.secondaryStartPos);
            togglePiece(move.castleMove.secondaryPieceType, move.castleMove.secondaryEndPos);
            break;
        case MoveType::EN_PASSANT:
            togglePiece(move.enPassantMove.killPieceType, move.enPassantMove.killSquare);    
            togglePiece(move.enPassantMove.pieceType, move.enPassantMove.startPos);
            togglePiece(move.enPassantMove.pieceType, move.enPassantMove.endPos);
            break;
        case MoveType::PROMOTION:
            togglePiece(move.promotionMove.killPieceType, move.promotionMove.endPos);
            togglePiece(move.promotionMove.oldPieceType, move.promotionMove.startPos);
            togglePiece(move.promotionMove.newPieceType, move.promotionMove.endPos);
            break;
        case MoveType::NORMAL:
            togglePiece(move.normalMove.killPieceType, move.normalMove.endPos);
            togglePiece(move.normalMove.pieceType, move.normalMove.startPos);
            togglePiece(move.normalMove.pieceType, move.normalMove.endPos);
            //set can castle flags if the king or a rook has moved
            break;
    }
    //set flags for things like check, etc
}
void Board::unMakeMove(const Move& move) {
    //surely this will need to differ from makeMove() at some point, right?
    switch (move.flag) {
        case MoveType::CASTLE:
            togglePiece(move.castleMove.primaryPieceType, move.castleMove.primaryStartPos);
            togglePiece(move.castleMove.primaryPieceType, move.castleMove.primaryEndPos);
            togglePiece(move.castleMove.secondaryPieceType, move.castleMove.secondaryStartPos);
            togglePiece(move.castleMove.secondaryPieceType, move.castleMove.secondaryEndPos);
            break;
        case MoveType::EN_PASSANT:
            togglePiece(move.enPassantMove.pieceType, move.enPassantMove.startPos);
            togglePiece(move.enPassantMove.pieceType, move.enPassantMove.endPos);
            togglePiece(move.enPassantMove.killPieceType, move.enPassantMove.killSquare);
            break;
        case MoveType::PROMOTION:
            togglePiece(move.promotionMove.killPieceType, move.promotionMove.endPos);
            togglePiece(move.promotionMove.oldPieceType, move.promotionMove.startPos);
            togglePiece(move.promotionMove.newPieceType, move.promotionMove.endPos);
            break;
        case MoveType::NORMAL:
            togglePiece(move.normalMove.killPieceType, move.normalMove.endPos);
            togglePiece(move.normalMove.pieceType, move.normalMove.startPos);
            togglePiece(move.normalMove.pieceType, move.normalMove.endPos);
            break;
    }
}

void Board::setupBoard(const std::string& FEN) {
    //TODO: this
    /*
        definitely needs 1, 3, and 4
        will probabably store 5 but this may be stored in Engine.cpp
        2 will be stored in Engine.cpp
        and 6 is useless
    */

    const std::string fenChars = "KQBNRPkqbnrp";

    int i;
    for (i = 0; i < FEN.length(); i++) {
        if (FEN[i] == ' ') break; //has read all of 1

        size_t index = fenChars.find(FEN[i]);

        if (index == -1) {
            i += (int)FEN[i];
            //should it be += (int)FEN[i] -1 because it will increase in the loop?
        }
        else {
            addPiece((PieceType)index, (SquareIndex)(((uint64_t)i)^0x38));
        }
    }
    for (; i < FEN.length(); i++) {
        //we're now talking about 2
    }
}

void Board::printBitBoard(PieceType board) {
    std::cout << std::bitset<64>(bitBoards[board]) << '\n';
}
void Board::printBitBoardHex(PieceType board) {
    printf("0x%016llx\n", bitBoards[board]);
}

// * ---------------------------------- [ PRIVATE METHODS ] ---------------------------------- * //

//To move a piece at index 5 up by 1 square, I reomve it from index 5, and add a piece at index 5+8 as per the compass rose on chessprogramming.org 
void Board::addPiece(PieceType type, SquareIndex index) {
    if (type == PieceType::INVALID) return;
    bitBoards[type] |= (1ULL << index);
    bitBoards[type <= 5 ? 12 : 13] |= (1ULL << index);
}
void Board::removePiece(PieceType type, SquareIndex index) {
    if (type == PieceType::INVALID) return;
    bitBoards[type] &= ~(1ULL << index);
    bitBoards[type <= 5 ? 12 : 13] &= ~(1ULL << index);
}
void Board::togglePiece(PieceType type, SquareIndex index) {
    if (type == PieceType::INVALID) return;
    bitBoards[type] ^= (1ULL << index);
    bitBoards[type <= 5 ? 12 : 13] ^= (1ULL << index);
}

void Board::setupDefaultBoard() {    
    bitBoards[PieceType::WHITE_PIECES]  = 0x0303030303030303;
    bitBoards[PieceType::WHITE_KING]    = 0x0000000100000000;
    bitBoards[PieceType::WHITE_QUEEN]   = 0x0000000001000000;
    bitBoards[PieceType::WHITE_BISHOP]  = 0x0000010000010000;
    bitBoards[PieceType::WHITE_KNIGHT]  = 0x0001000000000100;
    bitBoards[PieceType::WHITE_ROOK]    = 0x0100000000000001;
    bitBoards[PieceType::WHITE_PAWN]    = 0x0202020202020202;

    bitBoards[PieceType::BLACK_PIECES]  = 0xc0c0c0c0c0c0c0c0;
    bitBoards[PieceType::BLACK_KING]    = 0x0000008000000000;
    bitBoards[PieceType::BLACK_QUEEN]   = 0x0000000080000000;
    bitBoards[PieceType::BLACK_BISHOP]  = 0x0000800000800000;
    bitBoards[PieceType::BLACK_KNIGHT]  = 0x0080000000008000;
    bitBoards[PieceType::BLACK_ROOK]    = 0x8000000000000080;
    bitBoards[PieceType::BLACK_PAWN]    = 0x4040404040404040;
}

// * ---------------------------------- [ HELPER METHODS ] ---------------------------------- * //

bool Board::hasPiece(SquareIndex index) {
    return (bitBoards[PieceType::WHITE_PIECES] | bitBoards[PieceType::BLACK_PIECES]) & (1ULL << index);
}
bool Board::isWhite(SquareIndex index) {
    return bitBoards[PieceType::WHITE_PIECES] & (1ULL << index);
}
bool Board::isBlack(SquareIndex index) {
    return bitBoards[PieceType::BLACK_PIECES] & (1ULL << index);
}
