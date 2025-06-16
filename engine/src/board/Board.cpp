/*
    use LSR mapping
    use the same endian as this machine - little endian (i.e.: 1, 2, 4, 8, ...)
*/

#include "board/Board.hpp"

#include <iostream>
#include <bitset>

#include "board/Move.hpp"

//==================================[Constructors/Desctructors]====================================

Board::Board() {
    setupDefaultBoard();
}

Board::~Board() {
    delete bitBoards;
}

//===================================[Public methods]====================================

void Board::makeMove() {
    //TODO: this
}
void Board::unMakeMove() {
    //TODO: this
}

void Board::setupBoard() {
    //TODO: this
}

//======================================[Private methods]=================================

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