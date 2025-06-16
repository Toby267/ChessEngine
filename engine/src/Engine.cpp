#include "Engine.hpp"

#include <iostream>

#include "board/Board.hpp"

//==================================[Constructors/Desctructors]====================================

Engine::Engine() {
    board = new Board();
}

Engine::~Engine() {
    delete board;
}

//======================================[Private methods]=================================

void Engine::printASCIIBoard() {

}
