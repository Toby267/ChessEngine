#include "board/Board.hpp"

#include <cstdint>
#include <string>
#include <iostream>
#include <bitset>

#include "board/BoardUtil.hpp"
#include "board/Move.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ------------------------------------ [ CONSTRUCTORS/DESCTUCTOR ] ------------------------------------ * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

Board::Board() {
    setDefaultBoard();
}

Board::~Board() {
    
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ---------------------------------------- [ GETTERS/SETTERS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Returns the type of piece stored at the given index
 * 
 * @param index the index of said piece
 */
PieceType Board::getType(SquareIndex index) const {
    if (isWhite(index)) {
        for (int i = 0; i < 6; i++) {
            if (bitBoards[i] & (1ULL << index)) {
                return (PieceType)i;
            }
        }
    }
    else if (isBlack(index)) {
        for (int i = 6; i < 12; i++) {
            if (bitBoards[i] & (1ULL << index)) {
                return (PieceType)i;
            }
        }
    }
    
    return PieceType::INVALID;
}

/**
 * Getters returning const references
 */
const std::array<uint64_t, 14>& Board::getBitBoards() const {
    return bitBoards;
}
const std::array<__uint128_t,  4>& Board::getCastleData() const {
    return castleData;
}
const std::array<__uint128_t, 16>& Board::getEnPassantData() const {
    return enPassantData;
}
WhiteTurn Board::getWhiteTurn() const {
    return whiteTurn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ PUBLIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Logic for making a move on the bitboards, and setting relevent flags
 * 
 * @param move the move to be made
 */
void Board::makeMove(const Move& move) {
    whiteTurn = !whiteTurn;
    
    for (auto& i : castleData) {
        i <<= 1;
    }
    for (auto& i : enPassantData) {
        i <<= 1;
    }
    
    switch (move.flag) {
        case MoveType::CASTLE:
            togglePiece(move.castleMove.primaryPieceType, move.castleMove.primaryStartPos);
            togglePiece(move.castleMove.primaryPieceType, move.castleMove.primaryEndPos);
            togglePiece(move.castleMove.secondaryPieceType, move.castleMove.secondaryStartPos);
            togglePiece(move.castleMove.secondaryPieceType, move.castleMove.secondaryEndPos);
            updateSpecialMoveStatus(move);
            break;
            
        case MoveType::EN_PASSANT:
            togglePiece(move.enPassantMove.pieceType, move.enPassantMove.startPos);
            togglePiece(move.enPassantMove.pieceType, move.enPassantMove.endPos);
            togglePiece(move.enPassantMove.killPieceType, move.enPassantMove.killSquare);
            break;

        case MoveType::PROMOTION:
            togglePiece(move.promotionMove.killPieceType, move.promotionMove.endPos);
            togglePiece(move.promotionMove.oldPieceType, move.promotionMove.startPos);
            togglePiece(move.promotionMove.newPieceType, move.promotionMove.endPos);
            updateSpecialMoveStatus(move);
            break;

        case MoveType::NORMAL:
            togglePiece(move.normalMove.killPieceType, move.normalMove.endPos);
            togglePiece(move.normalMove.pieceType, move.normalMove.startPos);
            togglePiece(move.normalMove.pieceType, move.normalMove.endPos);
            updateSpecialMoveStatus(move);
            break;
    }
    //set flags for things like check, etc
}
/**
 * Logic for unmaking a move on the bitboards, and unsetting relevent flags
 * 
 * @param move the move to be unmade
 */
void Board::unMakeMove(const Move& move) {
    whiteTurn = !whiteTurn;
    
    for (auto& i : castleData) {
        i >>= 1;
    }
    for (auto& i : enPassantData) {
        i >>= 1;
    }

    switch (move.flag) {
        case MoveType::CASTLE:
            togglePiece(move.castleMove.primaryPieceType, move.castleMove.primaryStartPos);
            togglePiece(move.castleMove.primaryPieceType, move.castleMove.primaryEndPos);
            togglePiece(move.castleMove.secondaryPieceType, move.castleMove.secondaryStartPos);
            togglePiece(move.castleMove.secondaryPieceType, move.castleMove.secondaryEndPos);
            break;
        case MoveType::EN_PASSANT:
            togglePiece(move.enPassantMove.pieceType, move.enPassantMove.startPos);
            togglePiece(move.enPassantMove.pieceType, move.enPassantMove.endPos);
            togglePiece(move.enPassantMove.killPieceType, move.enPassantMove.killSquare);
            break;
        case MoveType::PROMOTION:
            togglePiece(move.promotionMove.killPieceType, move.promotionMove.endPos);
            togglePiece(move.promotionMove.oldPieceType, move.promotionMove.startPos);
            togglePiece(move.promotionMove.newPieceType, move.promotionMove.endPos);
            break;
        case MoveType::NORMAL:
            togglePiece(move.normalMove.killPieceType, move.normalMove.endPos);
            togglePiece(move.normalMove.pieceType, move.normalMove.startPos);
            togglePiece(move.normalMove.pieceType, move.normalMove.endPos);
            break;
    }
}

/**
 * Sets up the board in its starting position
 */
void Board::setDefaultBoard() {
    enPassantData = {};
    castleData = {};
    
    bitBoards[PieceType::WHITE_PIECES]  = 0x0303030303030303ULL;
    bitBoards[PieceType::WHITE_KING]    = 0x0000000100000000ULL;
    bitBoards[PieceType::WHITE_QUEEN]   = 0x0000000001000000ULL;
    bitBoards[PieceType::WHITE_BISHOP]  = 0x0000010000010000ULL;
    bitBoards[PieceType::WHITE_KNIGHT]  = 0x0001000000000100ULL;
    bitBoards[PieceType::WHITE_ROOK]    = 0x0100000000000001ULL;
    bitBoards[PieceType::WHITE_PAWN]    = 0x0202020202020202ULL;

    bitBoards[PieceType::BLACK_PIECES]  = 0xc0c0c0c0c0c0c0c0ULL;
    bitBoards[PieceType::BLACK_KING]    = 0x0000008000000000ULL;
    bitBoards[PieceType::BLACK_QUEEN]   = 0x0000000080000000ULL;
    bitBoards[PieceType::BLACK_BISHOP]  = 0x0000800000800000ULL;
    bitBoards[PieceType::BLACK_KNIGHT]  = 0x0080000000008000ULL;
    bitBoards[PieceType::BLACK_ROOK]    = 0x8000000000000080ULL;
    bitBoards[PieceType::BLACK_PAWN]    = 0x4040404040404040ULL;
}
/**
 * Resets the board back to its initial/default state
 */
void Board::resetBoard() {
    castleData = {1, 1, 1, 1};
    enPassantData = {};
    bitBoards = {};
}

/**
 * Parses a given fen, storing relevent information
 * 
 * @param FEN the fen that is to be parsed
*/
void Board::parseFen(const std::string& FEN) {
    resetBoard();

    const std::string fenChars = "KQBNRPkqbnrp";
    int i = 0;

    //parses the first part of the FEN
    for (int file = -1, rank = 7; i < FEN.length(); i++) {
        if (FEN[i] == ' ') break;
        
        size_t index = fenChars.find(FEN[i]);
        file++;

        if (FEN[i] == '/') {
            rank--;
            file = -1;
        }
        else if (index == -1) {
            file += FEN[i] - '1'; //'0'-1
        }
        else {
            addPiece((PieceType)index, (SquareIndex)(8*file+rank));
        }
    }

    //parses the second part of the FEN
    for (i++; i < FEN.length(); i++) {
        if (FEN[i] == ' ') break;

        whiteTurn = FEN[i] == 'w' ? true : false;
    }

    //parses the third part of the FEN
    for (i++; i < FEN.length(); i++) {
        if (FEN[i] == ' ') break;

        if      (FEN[i] == 'K') castleData[CastlePieces::W_KING]    = 0;
        else if (FEN[i] == 'Q') castleData[CastlePieces::W_QUEEN]   = 0;
        else if (FEN[i] == 'k') castleData[CastlePieces::B_KING]    = 0;
        else if (FEN[i] == 'q') castleData[CastlePieces::B_QUEEN]   = 0;
    }

    //parses the fourth part of the FEN
    if (FEN[++i] != '-') {
        int index = (FEN[i] - 'a') + (FEN[i+1] =='3' ? 0 : 8);
        enPassantData[(EnPassantPieces)(index)] = 1;
    }
}

/**
 * Prints debug data for use during development
 */
void Board::printDebugData() {
    for (auto& i : castleData)
        std::cout << std::bitset<128>(i) << '\n';
    std::cout << '\n';
    for (auto& i : enPassantData)
        std::cout << std::bitset<128>(i) << '\n';
    std::cout << "\n//////////////////////////////////////////////////////////////////////////////////////////////\n";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ---------------------------------------- [ PRIVATE METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Logic for determining which pieces can castle and which can en passant in a manor that can be undone with unMakeMove()
 * 
 * @param move the move to be played
 */
void Board::updateSpecialMoveStatus(const Move& move) {
    //logic for determining which pieces can en passant and which can castle
    switch (move.normalMove.pieceType) {
        case PieceType::WHITE_PAWN:
        case PieceType::BLACK_PAWN: {
            int dist = move.normalMove.endPos - move.normalMove.startPos;
            if (dist == 2 || dist == -2) {
                int index = (move.normalMove.startPos / 8) + ((move.normalMove.startPos & 7) == 1 ? 0 : 8);
                enPassantData[index] |= 0b1;
            }
            break;
        }

        case PieceType::WHITE_ROOK:
        case PieceType::BLACK_ROOK: {
            switch (move.normalMove.startPos) {
                case SquareIndex::a1:   { castleData[CastlePieces::W_QUEEN] |= 0b1;  break; }
                case SquareIndex::h1:   { castleData[CastlePieces::W_KING]  |= 0b1;  break; }
                case SquareIndex::a8:   { castleData[CastlePieces::B_QUEEN] |= 0b1;  break; }
                case SquareIndex::h8:   { castleData[CastlePieces::B_KING]  |= 0b1;  break; }
                default:                { break; }
            }
            break;
        }

        case PieceType::WHITE_KING: {
            castleData[CastlePieces::W_KING]     |= 0b1;
            castleData[CastlePieces::W_QUEEN]    |= 0b1;
            break;
        }
        case PieceType::BLACK_KING: {
            castleData[CastlePieces::B_KING]     |= 0b1;
            castleData[CastlePieces::B_QUEEN]    |= 0b1;
            break;
        }

        default: {
            break;
        }
    }

    //logic for determining if a rook has died and thus which pieces can castle    
    switch (move.normalMove.endPos) {
        case SquareIndex::a1:   { castleData[CastlePieces::W_QUEEN] |= 0b1;  break; }
        case SquareIndex::h1:   { castleData[CastlePieces::W_KING]  |= 0b1;  break; }
        case SquareIndex::a8:   { castleData[CastlePieces::B_QUEEN] |= 0b1;  break; }
        case SquareIndex::h8:   { castleData[CastlePieces::B_KING]  |= 0b1;  break; }
        default:                { break; }
    }
}

//adds a piece to a given square
void Board::addPiece(PieceType type, SquareIndex index) {
    if (type == PieceType::INVALID) return;
    bitBoards[type] |= (1ULL << index);
    bitBoards[type <= 5 ? PieceType::WHITE_PIECES : PieceType::BLACK_PIECES] |= (1ULL << index);
}
//removes a piece to a given square
void Board::removePiece(PieceType type, SquareIndex index) {
    if (type == PieceType::INVALID) return;
    bitBoards[type] &= ~(1ULL << index);
    bitBoards[type <= 5 ? PieceType::WHITE_PIECES : PieceType::BLACK_PIECES] &= ~(1ULL << index);
}
//toggles a piece in a given square
void Board::togglePiece(PieceType type, SquareIndex index) {
    if (type == PieceType::INVALID) return;
    bitBoards[type] ^= (1ULL << index);
    bitBoards[type <= 5 ? PieceType::WHITE_PIECES : PieceType::BLACK_PIECES] ^= (1ULL << index);
}

//prints the bitboard in binary
void Board::printBitBoardBin(PieceType board) {
    std::cout << std::bitset<64>(bitBoards[board]) << '\n';
}
//prints the bitboard in hex
void Board::printBitBoardHex(PieceType board) {
    printf("0x%016lx\n", bitBoards[board]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ HELPER METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//returns true if the given square has a piece
bool Board::hasPiece(SquareIndex index) {
    return (bitBoards[PieceType::WHITE_PIECES] | bitBoards[PieceType::BLACK_PIECES]) & (1ULL << index);
}
//returns true if the given square has a white piece
bool Board::isWhite(SquareIndex index) const {
    return bitBoards[PieceType::WHITE_PIECES] & (1ULL << index);
}
//returns true if the given square has a black piece
bool Board::isBlack(SquareIndex index) const {
    return bitBoards[PieceType::BLACK_PIECES] & (1ULL << index);
}
