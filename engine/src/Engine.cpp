#include "Engine.hpp"

#include <iostream>
#include <string>

#include "board/Board.hpp"

// * ---------------------------------- [ CONSTRUCTORS/DESCTUCTOR ] ---------------------------------- * //

Engine::Engine() {
    board = new Board();
    printASCIIBoard();

    Move myMove = {MoveType::NORMAL, NormalMove{SquareIndex::a1, SquareIndex::c7, PieceType::WHITE_ROOK, PieceType::BLACK_PAWN}};
    
    board->makeMove(myMove);
    printASCIIBoard();
    
    board->makeMove(myMove);
    printASCIIBoard();
}

Engine::~Engine() {
    delete board;
}

// * ---------------------------------- [ PRIVATE METHODS ] ----------------------------------- * //

void Engine::printASCIIBoard() {
    const std::string pieceChars[] = {"♚", "♛", "♝", "♞", "♜", "♟", "♔", "♕", "♗", "♘", "♖", "♙"};
    
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            PieceType type = board->getType((SquareIndex)(8*file+rank));

            if (type == 14)
                std::cout << "o" << ' ';
            else
                std::cout << pieceChars[(int)type] << ' ';
        }
        std::cout << '\n';
    }

    std::cout << '\n';
}
