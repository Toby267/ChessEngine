/**
 * To move a piece at index 5 up by 1 square, I reomve it from index 5, and add a piece at index 5+8 as per the compass rose on chessprogramming.org
 * 
 * TODO:
 * Move generation (without checking for check and the like)
 * Bitscan for converting move bitboards to move structs
 * Arbiter to make move generation check for check and the like
 * Validate moves
 * Game endings
 */

#include <iostream>

#include "Engine.hpp"

int main(int argc, char *argv[]) {
    Engine engine{};

    return 0;
}
