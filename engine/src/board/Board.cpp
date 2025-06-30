#include "board/Board.hpp"

#include <cstdint>
#include <string>
#include <optional>
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
PieceType Board::getType(SquareIndex index) {
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
 * Returns a const reference to the bitboards array
 */
const std::array<uint64_t, 14>& Board::getBitBoards() const {
    return bitBoards;
}

/**
 * Sets a specific bitboard to the given value
 */
void Board::setBitBoard(uint64_t val, PieceType type) {
    bitBoards[type] = val;
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
    for (auto& i : enPassantSquares) {
        i <<= 1;
    }
    whiteCastleKing <<= 1, whiteCastleQueen <<= 1;
    blackCastleKing <<= 1, blackCastleQueen <<= 1;
    
    switch (move.flag) {
        case MoveType::CASTLE:
            togglePiece(move.castleMove.primaryPieceType, move.castleMove.primaryStartPos);
            togglePiece(move.castleMove.primaryPieceType, move.castleMove.primaryEndPos);
            togglePiece(move.castleMove.secondaryPieceType, move.castleMove.secondaryStartPos);
            togglePiece(move.castleMove.secondaryPieceType, move.castleMove.secondaryEndPos);
            break;
            
        case MoveType::EN_PASSANT:
            togglePiece(move.enPassantMove.killPieceType, move.enPassantMove.killSquare);    
            togglePiece(move.enPassantMove.pieceType, move.enPassantMove.startPos);
            togglePiece(move.enPassantMove.pieceType, move.enPassantMove.endPos);
            break;

        case MoveType::PROMOTION:
            togglePiece(move.promotionMove.killPieceType, move.promotionMove.endPos);
            togglePiece(move.promotionMove.oldPieceType, move.promotionMove.startPos);
            togglePiece(move.promotionMove.newPieceType, move.promotionMove.endPos);
            checkDeadRook(move.promotionMove);
            break;

        case MoveType::NORMAL:
            togglePiece(move.normalMove.killPieceType, move.normalMove.endPos);
            togglePiece(move.normalMove.pieceType, move.normalMove.startPos);
            togglePiece(move.normalMove.pieceType, move.normalMove.endPos);
            updateSpecialMoveStatus(move.normalMove);
            checkDeadRook(move.normalMove);
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
    for (auto& i : enPassantSquares) {
        i >>= 1;
    }
    whiteCastleKing >>= 1, whiteCastleQueen >>= 1;
    blackCastleKing >>= 1, blackCastleQueen >>= 1;

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
    enPassantSquares[16] = {0};

    whiteCastleKing = 0, whiteCastleQueen = 0;
    blackCastleKing = 0, blackCastleQueen = 0;
    
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
    enPassantSquares[16] = {0};

    whiteCastleKing = 1, whiteCastleQueen = 1;
    blackCastleKing = 1, blackCastleQueen = 1;
    
    for (uint64_t& board : bitBoards)
        board = 0;
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

    //skips the second part of the FEN
    for (i++; i < FEN.length() && FEN[i] != ' '; i++)
        ;

    //parses the third part of the FEN
    for (i++; i < FEN.length(); i++) {
        if (FEN[i] == ' ') break;

        if      (FEN[i] == 'K') whiteCastleKing     = 0;
        else if (FEN[i] == 'Q') whiteCastleQueen    = 0;
        else if (FEN[i] == 'k') blackCastleKing     = 0;
        else if (FEN[i] == 'q') blackCastleQueen    = 0;
    }

    //parses the fourth part of the FEN
    if (FEN[++i] != '-') {
        int index = (FEN[i] - 'a') + (FEN[i+1] =='3' ? 0 : 8);
        enPassantSquares[(EnPassantPieces)(index)] = 1;
    }
}

/**
 * Prints debug data for use during development
 */
void Board::printDebugData() {
    printBitBoardHex(PieceType::WHITE_PAWN);
    printf("0x%016llx\n", northOne(bitBoards[PieceType::WHITE_PAWN]));

    printBitBoardHex(PieceType::WHITE_PAWN);
    printf("0x%016llx\n", southOne(bitBoards[PieceType::WHITE_PAWN]));

    printBitBoardHex(PieceType::WHITE_PAWN);
    printf("0x%016llx\n", eastOne(bitBoards[PieceType::WHITE_PAWN]));

    printBitBoardHex(PieceType::WHITE_PAWN);
    printf("0x%016llx\n", westOne(bitBoards[PieceType::WHITE_PAWN]));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ---------------------------------------- [ PRIVATE METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Logic for determining which pieces can castle and which can en passant in a manor that can be undone with unMakeMove()
 * 
 * @param move the move to be played
 */
void Board::updateSpecialMoveStatus(const NormalMove& move) {
    switch (move.pieceType) {
        case PieceType::WHITE_PAWN:
        case PieceType::BLACK_PAWN: {
            int dist = move.endPos - move.startPos;
            if (dist == 2 || dist == -2) {
                int index = (move.startPos / 8) + (move.startPos & 7 == 1 ? 0 : 8);
                enPassantSquares[index] &= 0b1;
            }
            break;
        }

        case PieceType::WHITE_ROOK:
        case PieceType::BLACK_ROOK: {
            switch (move.startPos) {
                case SquareIndex::a1:   { whiteCastleQueen &= 0b1;  break; }
                case SquareIndex::h1:   { whiteCastleKing  &= 0b1;  break; }
                case SquareIndex::a8:   { blackCastleQueen &= 0b1;  break; }
                case SquareIndex::h8:   { blackCastleKing  &= 0b1;  break; }
            }
        }

        case PieceType::WHITE_KING: {
            whiteCastleKing     &= 0b1;
            whiteCastleQueen    &= 0b1;
            break;
        }
        case PieceType::BLACK_KING: {
            blackCastleKing     &= 0b1;
            blackCastleQueen    &= 0b1;
            break;
        }
    }
}

/**
 * Logic for determining if a rook has died and thus which pieces can castle
 * 
 * @param move the move to be played
 */
void Board::checkDeadRook(const NormalMove& move) {
    if (move.killPieceType != PieceType::WHITE_ROOK && move.killPieceType != PieceType::BLACK_ROOK)
        return;
    
    switch (move.startPos) {
        case SquareIndex::a1:   { whiteCastleQueen &= 0b1;  break; }
        case SquareIndex::h1:   { whiteCastleKing  &= 0b1;  break; }
        case SquareIndex::a8:   { blackCastleQueen &= 0b1;  break; }
        case SquareIndex::h8:   { blackCastleKing  &= 0b1;  break; }
    }
}
void Board::checkDeadRook(const PromotionMove& move) {
    if (move.killPieceType != PieceType::WHITE_ROOK && move.killPieceType != PieceType::BLACK_ROOK)
        return;
    
    switch (move.startPos) {
        case SquareIndex::a1:   { whiteCastleQueen &= 0b1;  break; }
        case SquareIndex::h1:   { whiteCastleKing  &= 0b1;  break; }
        case SquareIndex::a8:   { blackCastleQueen &= 0b1;  break; }
        case SquareIndex::h8:   { blackCastleKing  &= 0b1;  break; }
    }
}

//adds a piece to a given square
void Board::addPiece(PieceType type, SquareIndex index) {
    if (type == PieceType::INVALID) return;
    bitBoards[type] |= (1ULL << index);
    bitBoards[type <= 5 ? 12 : 13] |= (1ULL << index);
}
//removes a piece to a given square
void Board::removePiece(PieceType type, SquareIndex index) {
    if (type == PieceType::INVALID) return;
    bitBoards[type] &= ~(1ULL << index);
    bitBoards[type <= 5 ? 12 : 13] &= ~(1ULL << index);
}
//toggles a piece in a given square
void Board::togglePiece(PieceType type, SquareIndex index) {
    if (type == PieceType::INVALID) return;
    bitBoards[type] ^= (1ULL << index);
    bitBoards[type <= 5 ? 12 : 13] ^= (1ULL << index);
}

//prints the bitboard in binary
void Board::printBitBoardBin(PieceType board) {
    std::cout << std::bitset<64>(bitBoards[board]) << '\n';
}
//prints the bitboard in hex
void Board::printBitBoardHex(PieceType board) {
    printf("0x%016llx\n", bitBoards[board]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ HELPER METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//returns true if the given square has a piece
bool Board::hasPiece(SquareIndex index) {
    return (bitBoards[PieceType::WHITE_PIECES] | bitBoards[PieceType::BLACK_PIECES]) & (1ULL << index);
}
//returns true if the given square has a white piece
bool Board::isWhite(SquareIndex index) {
    return bitBoards[PieceType::WHITE_PIECES] & (1ULL << index);
}
//returns true if the given square has a black piece
bool Board::isBlack(SquareIndex index) {
    return bitBoards[PieceType::BLACK_PIECES] & (1ULL << index);
}
