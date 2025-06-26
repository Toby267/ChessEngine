#include "board/Board.hpp"

#include <cstdint>
#include <string>
#include <optional>
#include <iostream>
#include <bitset>

#include "board/BoardEnums.hpp"
#include "board/Move.hpp"

// * ---------------------------------- [ CONSTRUCTORS/DESCTUCTOR ] ---------------------------------- * //

Board::Board() {
    setDefaultBoard();
}

Board::~Board() {
    
}

// * ---------------------------------- [ GETTERS/SETTERS ] ---------------------------------- * //

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

// * ------------------------------------- [ PUBLIC METHODS ] --------------------------------------- * //

/**
 * Logic for making a move on the bitboards, and setting relevent flags
 * 
 * @param move the move to be made
 */
void Board::makeMove(const Move& move) {
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
            break;
        case MoveType::NORMAL:
            togglePiece(move.normalMove.killPieceType, move.normalMove.endPos);
            togglePiece(move.normalMove.pieceType, move.normalMove.startPos);
            togglePiece(move.normalMove.pieceType, move.normalMove.endPos);
            //set can castle flags if the king or a rook has moved
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
    //surely this will need to differ from makeMove() at some point, right?
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
    enPassantSquare = std::nullopt;

    whiteCastleKing = true, whiteCastleQueen = true;
    blackCastleKing = true, blackCastleQueen = true;
    
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
    enPassantSquare = std::nullopt;

    whiteCastleKing = false, whiteCastleQueen = false;
    blackCastleKing = false, blackCastleQueen = false;

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

        if      (FEN[i] == 'K') whiteCastleKing     = true;
        else if (FEN[i] == 'Q') whiteCastleQueen    = true;
        else if (FEN[i] == 'k') blackCastleKing     = true;
        else if (FEN[i] == 'q') blackCastleQueen    = true;
    }

    //parses the fourth part of the FEN
    if (FEN[++i] != '-') {
        int squareIndex = 8 * (FEN[i] - 'a') + FEN[i+1] - '1';
        enPassantSquare = (SquareIndex)squareIndex;
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
    
    // std::cout << whiteCastleKing << whiteCastleQueen << blackCastleKing << blackCastleQueen << '\n';
    // if (enPassantSquare.has_value())
    //     std::cout << enPassantSquare.value() << '\n';
    // else
    //     std::cout << '-' << '\n';
}

// * ---------------------------------- [ PRIVATE METHODS ] ---------------------------------- * //

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

// * ---------------------------------- [ HELPER METHODS ] ---------------------------------- * //

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
