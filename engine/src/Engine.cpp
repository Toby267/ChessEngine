#include "Engine.hpp"

#include <iostream>

#include "board/Board.hpp"

// * ---------------------------------- [ CONSTRUCTORS/DESCTUCTOR ] ---------------------------------- * //

Engine::Engine() {
    board = new Board();
}

Engine::~Engine() {
    delete board;
}

// * ---------------------------------- [ PRIVATE/HELPER METHODS ] ---------------------------------- * //

void Engine::printASCIIBoard() {

}
