/*
    use LSR mapping
    use the same endian as this machine - little endian (i.e.: 1, 2, 4, 8, ...)
*/

#include "board/Board.hpp"

#include <iostream>

Board::Board() {
    std::cout << "within board constructor" << '\n';
}

Board::~Board() {

}