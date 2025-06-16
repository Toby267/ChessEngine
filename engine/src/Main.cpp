//Could make the move generator class static and reference it in Engine.cpp for the move validator

#include <iostream>

#include "Engine.hpp"

int main(int argc, char *argv[]) {
    std::cout << "CMake test" << '\n';

    Engine engine;

    return 0;
}

/*
Board board0;
Board board1{};
Board board2(5);
Board *board3 = new Board(5);
Board &board4 = *new Board(5);

//board1 on stack so no delete
delete board2;
delete &board3;

//apparently you should not use 4
*/
