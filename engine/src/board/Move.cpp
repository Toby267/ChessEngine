#include "board/Move.hpp"
#include "board/BoardUtil.hpp"

#include <iostream>

/**
 * Prints all data related to a given move
 * 
 * @param move the move to be printed
 */
void Move::print() {
    switch (flag) {
        case NORMAL: {
            NormalMove m = normalMove;
            std::cout << "Start pos: "<< m.startPos << ", End pos: " << m.endPos << '\n';
            std::cout << "Piece type: " << m.pieceType << ", Kill piece type: " << m.killPieceType  << '\n';
            break;
        }
        case PROMOTION: {
            PromotionMove m = promotionMove;
            std::cout << "Start pos: " << m.startPos << ", End pos: " << m.endPos << '\n';
            std::cout << "Old piece type: "  << m.oldPieceType << ", New piece type: " << m.newPieceType << '\n';
            std::cout << "Kill piece type: " << m.killPieceType << '\n';
            break;
        }
        case EN_PASSANT: {
            EnPassantMove m = enPassantMove;
            std::cout << "Start pos: " << m.startPos << ", End pos: " << m.endPos << '\n';
            std::cout << "Piece type: " << m.pieceType << '\n';
            std::cout << "Kill piece type: " << m.killPieceType << ", Kill piece square: " << m.killPieceType << '\n';
            break;
        }
        case CASTLE: {
            CastleMove m = castleMove;
            std::cout << "Primary start pos: " << m.primaryStartPos << ", Primary end pos: " << m.primaryEndPos << ", Primary piece type: " << m.primaryPieceType << '\n';
            std::cout << "Secondary start pos: " << m.secondaryStartPos << ", Secondary end pos: " << m.secondaryEndPos << ", Secondary piece type: " << m.secondaryPieceType << '\n';
            break;
        }
        default: {
            std::cout << "Invalid Move struct" << '\n';
        }
    }
}

std::string Move::toString() {
    std::string moveStr     = {
        (char)(normalMove.startPos / 8 + 'a'), (char)(normalMove.startPos % 8 + '1'),
        (char)(normalMove.endPos   / 8 + 'a'), (char)(normalMove.endPos   % 8 + '1')
    };

    if (flag == PROMOTION) {    
        switch (promotionMove.newPieceType) {
            case PieceType::WHITE_QUEEN:
            case PieceType::BLACK_QUEEN:
                moveStr.append("queen");
                break;
            case PieceType::WHITE_ROOK:
            case PieceType::BLACK_ROOK:
                moveStr.append("rook");
                break;
            case PieceType::WHITE_BISHOP:
            case PieceType::BLACK_BISHOP:
                moveStr.append("bishop");
                break;
            case PieceType::WHITE_KNIGHT:
            case PieceType::BLACK_KNIGHT:
                moveStr.append("knight");
                break;
            default:
                break;
        }
    }

    return moveStr;
}
