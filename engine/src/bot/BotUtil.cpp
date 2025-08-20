#include "bot/BotUtil.hpp"

#include <iostream>

void pVariation::print() const {
    for (int i = 0; i < moveCount; i++) {
        std::cout << moves[i].toString() << '\n';
    }
}
