#pragma once

#include "board/Move.hpp"

typedef struct pVariation {
    int moveCount;
    Move moves[15];

    void print() const;
} pVariation;