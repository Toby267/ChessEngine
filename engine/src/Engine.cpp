#include "Engine.hpp"

#include <iostream>
#include <string>

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
    const std::string ascii[] = {"KI", "QU", "BI", "KN", "RO", "PA", "ki", "qu", "bi", "kn", "ro", "pa"};
    
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            PieceType type = board->getType((SquareIndex)(8*file+rank));

            if (type == 14)
                std::cout << "[]" << '\t';
            else
                std::cout << ascii[(int)type] << '\t';
        }
        std::cout << '\n';
    }
}
