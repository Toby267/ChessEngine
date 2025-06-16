#include "Engine.hpp"

#include <iostream>

Engine::Engine() {
    std::cout << "within engine constructor" << '\n';
    board = new Board();
}

Engine::~Engine() {

}
