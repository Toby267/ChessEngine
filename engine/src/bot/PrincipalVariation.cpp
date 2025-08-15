#include "bot/PrincipalVariation.hpp"

#include <iostream>

void pVariation::print() {
    for (int i = 0; i < moveCount; i++)
        std::cout << moves[i].toString() << '\n';
}