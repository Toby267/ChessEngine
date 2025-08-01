#include "board/Move.hpp"

void printMove(const Move& m) {
    switch (m.flag) {
        case NORMAL:
            std::cout << "Start: " << m.normalMove.startPos << ", End: " << m.normalMove.endPos << '\n';
            break;
        case PROMOTION:
            std::cout << "Start: " << m.promotionMove.startPos << ", End: " << m.promotionMove.endPos << '\n';
            break;
        case EN_PASSANT:
            std::cout << "Start: " << m.enPassantMove.startPos << ", End: " << m.enPassantMove.endPos << '\n';
            break;
        case CASTLE:
            std::cout << "Start: " << m.castleMove.primaryStartPos << ", End: " << m.castleMove.primaryEndPos << '\n';
            break;
    }
}