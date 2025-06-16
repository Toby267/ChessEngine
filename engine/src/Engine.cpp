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

// * ---------------------------------- [ PRIVATE METHODS ] ----------------------------------- * //

void Engine::printASCIIBoard() {
    //TODO: this
}
