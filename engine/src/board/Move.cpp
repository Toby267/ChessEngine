#include "board/Move.hpp"
#include "board/BoardUtil.hpp"

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

std::string moveToString(const Move& move) {
    std::string moveStr     = {
        (char)(move.normalMove.startPos / 8 + 'a'), (char)(move.normalMove.startPos % 8 + '1'),
        (char)(move.normalMove.endPos   / 8 + 'a'), (char)(move.normalMove.endPos   % 8 + '1')
    };

    if (move.flag == PROMOTION) {    
        switch (move.promotionMove.newPieceType) {
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
