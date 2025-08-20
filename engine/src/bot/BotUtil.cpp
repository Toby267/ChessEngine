#include "bot/BotUtil.hpp"
#include "board/Board.hpp"
#include "board/BoardUtil.hpp"

#include <cstdint>
#include <iostream>
#include <random>

#define ZOBRIST_COUNT (781)
static uint64_t zobristValues[ZOBRIST_COUNT];
/*
000-768 - piecevalues
769     - is black to move
770-773 - castling rights
774-781 - en passant rights
*/

void Zobrist::initZobristValues() {
    std::random_device randomDevice;
    std::mt19937_64 engine(randomDevice());
    std::uniform_int_distribution<uint64_t> range(0, UINT64_MAX);

    for (int i = 0; i < ZOBRIST_COUNT; i++)
        zobristValues[i] = range(engine);
}

int generateKey(const Board& board) {
    //side to move
    uint64_t retval = 0;
    
    //castling rights
    //en passant rights
    
    //piece values
    const std::array<int, 64>& mailboxBoard = board.getMailboxBoard();
    for (int i = 0; i < 64; i++) {
        if (mailboxBoard[i] == PieceType::INVALID)
            continue;
        retval ^= zobristValues[(mailboxBoard[i]+1) * (i+1)];
    }

    return retval;
}

void pVariation::print() const {
    for (int i = 0; i < moveCount; i++) {
        std::cout << moves[i].toString() << '\n';
    }
}
