#include "board/Move.hpp"
#include "board/BoardUtil.hpp"

#include <iostream>

/**
 * Prints all data related to a given move
 * 
 * @param move the move to be printed
 */
void Move::print() const {
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

std::string Move::toString() const {
    std::string moveStr     = {
        (char)(normalMove.startPos / 8 + 'a'), (char)(normalMove.startPos % 8 + '1'),
        (char)(normalMove.endPos   / 8 + 'a'), (char)(normalMove.endPos   % 8 + '1')
    };

    if (flag == PROMOTION) {    
        switch (promotionMove.newPieceType) {
            case PieceType::WHITE_QUEEN:
            case PieceType::BLACK_QUEEN:
                moveStr.append("q");
                break;
            case PieceType::WHITE_ROOK:
            case PieceType::BLACK_ROOK:
                moveStr.append("r");
                break;
            case PieceType::WHITE_BISHOP:
            case PieceType::BLACK_BISHOP:
                moveStr.append("b");
                break;
            case PieceType::WHITE_KNIGHT:
            case PieceType::BLACK_KNIGHT:
                moveStr.append("n");
                break;
            default:
                break;
        }
    }

    return moveStr;
}

bool Move::operator ==(const Move& other) const {
    if (flag != other.flag) return false;

    switch (flag) {
        case NORMAL: {
            if (normalMove.startPos             != other.normalMove.startPos)           return false;
            if (normalMove.endPos               != other.normalMove.endPos)             return false;
            if (normalMove.pieceType            != other.normalMove.pieceType)          return false;
            if (normalMove.killPieceType        != other.normalMove.killPieceType)      return false;
            return true;
        }
        case PROMOTION: {
            if (promotionMove.startPos          != other.promotionMove.startPos)        return false;
            if (promotionMove.endPos            != other.promotionMove.endPos)          return false;
            if (promotionMove.oldPieceType      != other.promotionMove.oldPieceType)    return false;
            if (promotionMove.newPieceType      != other.promotionMove.newPieceType)    return false;
            if (promotionMove.killPieceType     != other.promotionMove.killPieceType)   return false;
            return true;
        }
        case EN_PASSANT: {
            if (enPassantMove.startPos          != other.enPassantMove.startPos)        return false;
            if (enPassantMove.endPos            != other.enPassantMove.endPos)          return false;
            if (enPassantMove.pieceType         != other.enPassantMove.pieceType)       return false;
            if (enPassantMove.killSquare        != other.enPassantMove.killSquare)      return false;
            if (enPassantMove.killPieceType     != other.enPassantMove.killPieceType)   return false;
            return true;
        }
        case CASTLE: {
            if (castleMove.primaryStartPos      != other.castleMove.primaryStartPos)    return false;
            if (castleMove.primaryEndPos        != other.castleMove.primaryEndPos)      return false;
            if (castleMove.primaryPieceType     != other.castleMove.primaryPieceType)   return false;
            if (castleMove.secondaryStartPos    != other.castleMove.secondaryStartPos)  return false;
            if (castleMove.secondaryEndPos      != other.castleMove.secondaryEndPos)    return false;
            if (castleMove.secondaryPieceType   != other.castleMove.secondaryPieceType) return false;
            return true;
        }
        default: {
            return false;
        }
    }

    // if (flag != other.flag) return false;

    // if (normalMove.startPos != other.normalMove.startPos)   return false;
    // if (normalMove.endPos   != other.normalMove.endPos)     return false;

    // if (flag == PROMOTION && promotionMove.newPieceType != other.promotionMove.newPieceType) return false;
}
