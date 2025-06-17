#include "board/Board.hpp"

#include <iostream>
#include <bitset>
#include <inttypes.h>

#include "board/Move.hpp"

// * ---------------------------------- [ CONSTRUCTORS/DESCTUCTOR ] ---------------------------------- * //

Board::Board() {
    setupDefaultBoard();
}

Board::~Board() {
    
}

// * ---------------------------------- [ GETTERS/SETTERS ] ---------------------------------- * //

PieceType Board::getType(const SquareIndex index) {
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

void Board::makeMove() {
    //TODO: this
}
void Board::unMakeMove() {
    //TODO: this
}

void Board::setupBoard() {
    //TODO: this
}

// * ---------------------------------- [ PRIVATE METHODS ] ---------------------------------- * //

//To move a piece at index 5 up by 1 square, I reomve it from index 5, and add a piece at index 5+8 as per the compass rose on chessprogramming.org 
void Board::addPiece(const PieceType type, const SquareIndex index) {
    bitBoards[type] |= (1ULL << index);
    bitBoards[type <= 5 ? 12 : 13] |= (1ULL << index);
}
void Board::removePiece(const PieceType type, const SquareIndex index) {
    bitBoards[type] &= ~(1ULL << index);
    bitBoards[type <= 5 ? 12 : 13] &= ~(1ULL << index);
}
void Board::togglePiece(const PieceType type, const SquareIndex index) {
    bitBoards[type] ^= (1ULL << index);
    bitBoards[type <= 5 ? 12 : 13] ^= (1ULL << index);
}

void Board::printBitBoard(const PieceType board) {
    std::cout << std::bitset<64>(bitBoards[board]) << '\n';
}
void Board::printBitBoardHex(const PieceType board) {
    printf("0x%016llx\n", bitBoards[board]);
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

bool Board::hasPiece(const SquareIndex index) {
    return (bitBoards[PieceType::WHITE_PIECES] | bitBoards[PieceType::BLACK_PIECES]) & (1ULL << index);
}
bool Board::isWhite(const SquareIndex index) {
    return bitBoards[PieceType::WHITE_PIECES] & (1ULL << index);
}
bool Board::isBlack(const SquareIndex index) {
    return bitBoards[PieceType::BLACK_PIECES] & (1ULL << index);
}
