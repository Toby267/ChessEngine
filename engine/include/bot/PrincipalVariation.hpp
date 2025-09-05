#pragma once

#include "board/Move.hpp"
#include <atomic>
#include <cstddef>

typedef struct pVariation {
    int moveCount{};
    Move moves[15];

    void print() const;
} pVariation;

typedef struct concurrencyTreeNode {
    std::atomic<bool> val;
    concurrencyTreeNode* firstKid;
    concurrencyTreeNode* nextSibling;

    concurrencyTreeNode(bool value) {
        val = value;
        firstKid = nullptr;
        nextSibling = nullptr;
    }
    concurrencyTreeNode(concurrencyTreeNode* parent, bool value) {
        val = value;
        firstKid = nullptr;
        nextSibling = nullptr;

        concurrencyTreeNode* p = parent->firstKid;
        if (p == nullptr) {
            parent->firstKid = this;
            return;
        }

        while (p->nextSibling != nullptr)
            p = p->nextSibling;

        p->nextSibling = this;
    }
    void reset(bool value) {
        val = value;
        firstKid = nullptr;
        nextSibling = nullptr;
    }
    void setTrue() {
        val = true;
        if (firstKid != nullptr)
            firstKid->propogateTrue();
    }

private:
    void propogateTrue() {
        val = true;
        if (firstKid != nullptr)
            firstKid->propogateTrue();
        
        for (concurrencyTreeNode* p = nextSibling; p != nullptr; p = p->nextSibling)
            p->propogateTrue();

    }
} concurrencyTreeNode;
