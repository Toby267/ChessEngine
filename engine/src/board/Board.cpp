/*
    use LSR mapping
    use the same endian as this machine - little endian (i.e.: 1, 2, 4, 8, ...)
*/

#include "board/Board.hpp"

#include <iostream>
#include <bitset>

#include "board/Move.hpp"

// * ---------------------------------- [ CONSTRUCTORS/DESCTUCTOR ] ---------------------------------- * //

Board::Board() {
    setupDefaultBoard();
}

Board::~Board() {
    delete bitBoards;
}

// * ---------------------------------- [ GETTERS/SETTERS ] ---------------------------------- * //

//TODO: test this
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
    else {
        return PieceType::INVALID;
    }   
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

void Board::addPiece(PieceType type, SquareIndex index) {
    bitBoards[type] |= (1ULL << index);
    bitBoards[type <= 5 ? 12 : 13] |= (1ULL << index);
}
void Board::removePiece(PieceType type, SquareIndex index) {
    bitBoards[type] &= ~(1ULL << index);
    bitBoards[type <= 5 ? 12 : 13] &= ~(1ULL << index);
}
void Board::togglePiece(PieceType type, SquareIndex index) {
    bitBoards[type] ^= (1ULL << index);
    bitBoards[type <= 5 ? 12 : 13] ^= (1ULL << index);
}

void Board::printBitBoard(PieceType board) {
    std::cout << std::bitset<64>(bitBoards[board]) << '\n';
}

void Board::setupDefaultBoard() {
    //TODO: this
}

// * ---------------------------------- [ HELPER METHODS ] ---------------------------------- * //

//TODO: test this
bool Board::hasPiece(SquareIndex index) {
    return (bitBoards[WHITE_PIECES] | bitBoards[BLACK_PIECES]) & (1ULL << index);
}
//TODO: test this
bool Board::isWhite(SquareIndex index) {
    return bitBoards[WHITE_PIECES] & (1ULL << index);
}
//TODO: test this
bool Board::isBlack(SquareIndex index) {
    return bitBoards[BLACK_PIECES] & (1ULL << index);
}
