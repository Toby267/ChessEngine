#include "board/Move.hpp"

#include <iostream>

/**
 * Prints all data related to a given move
 * 
 * @param move the move to be printed
 */
void printMove(const Move& move) {
    switch (move.flag) {
        case NORMAL: {
            NormalMove m = move.normalMove;
            std::cout << "Start pos: "<< m.startPos << ", End pos: " << m.endPos << '\n';
            std::cout << "Piece type: " << m.pieceType << ", Kill piece type: " << m.killPieceType  << '\n';
            break;
        }
        case PROMOTION: {
            PromotionMove m = move.promotionMove;
            std::cout << "Start pos: " << m.startPos << ", End pos: " << m.endPos << '\n';
            std::cout << "Old piece type: "  << m.oldPieceType << ", New piece type: " << m.newPieceType << '\n';
            std::cout << "Kill piece type: " << m.killPieceType << '\n';
            break;
        }
        case EN_PASSANT: {
            EnPassantMove m = move.enPassantMove;
            std::cout << "Start pos: " << m.startPos << ", End pos: " << m.endPos << '\n';
            std::cout << "Piece type: " << m.pieceType << '\n';
            std::cout << "Kill piece type: " << m.killPieceType << ", Kill piece square: " << m.killPieceType << '\n';
            break;
        }
        case CASTLE: {
            CastleMove m = move.castleMove;
            std::cout << "Primary start pos: " << m.primaryStartPos << ", Primary end pos: " << m.primaryEndPos << ", Primary piece type: " << m.primaryPieceType << '\n';
            std::cout << "Secondary start pos: " << m.secondaryStartPos << ", Secondary end pos: " << m.secondaryEndPos << ", Secondary piece type: " << m.secondaryPieceType << '\n';
            break;
        }
        default: {
            std::cout << "Invalid Move struct" << '\n';
        }
    }
}
