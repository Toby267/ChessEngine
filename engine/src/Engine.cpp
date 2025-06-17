#include "Engine.hpp"

#include <iostream>

#include "board/Board.hpp"

// * ---------------------------------- [ CONSTRUCTORS/DESCTUCTOR ] ---------------------------------- * //

Engine::Engine() {
    board = new Board();
    printASCIIBoard();
}

Engine::~Engine() {
    delete board;
}

// * ---------------------------------- [ PRIVATE METHODS ] ----------------------------------- * //

void Engine::printASCIIBoard() {
    const char pieceChars[] = {'♚', '♛', '♝', '♞', '♜', '♟', '♔', '♕', '♗', '♘', '♖', '♙'};
    
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
}
