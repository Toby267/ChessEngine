#include "board/Board.hpp"

#include <cstdint>
#include <string>
#include <iostream>
#include <bitset>

#include "board/BoardUtil.hpp"
#include "board/Move.hpp"
#include "moveGeneration/MoveGenerator.hpp"

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


//const ref getters
const std::array<uint64_t, 14>& Board::getBitBoards() const {
    return bitBoards;
}
const std::bitset<64*14> Board::getBitBoardsAsBitset() const {
    std::bitset<64*14> set;

    for (int i = 0; i < 14; i++)
        for (int j = 0; j < 64; j++)
            set[i*14 + j] = (bitBoards[i] >> j) & 1;

    return set;
}
const std::array<int, 64>& Board::getMailboxBoard() const {
    return mailBoxBoard;
}
const std::array<__uint128_t,  4>& Board::getCastleData() const {
    return castleData;
}
const std::array<__uint128_t, 16>& Board::getEnPassantData() const {
    return enPassantData;
}
//const getters
PieceType::Enum Board::getType(SquareIndex index) const {
    return (PieceType::Enum)(mailBoxBoard[index]);
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
            togglePiece(move.castleMove.secondaryPieceType, move.castleMove.secondaryEndPos);
            togglePiece(move.castleMove.secondaryPieceType, move.castleMove.secondaryStartPos);
            togglePiece(move.castleMove.primaryPieceType, move.castleMove.primaryEndPos);
            togglePiece(move.castleMove.primaryPieceType, move.castleMove.primaryStartPos);
            break;
        case MoveType::EN_PASSANT:
            togglePiece(move.enPassantMove.killPieceType, move.enPassantMove.killSquare);
            togglePiece(move.enPassantMove.pieceType, move.enPassantMove.endPos);
            togglePiece(move.enPassantMove.pieceType, move.enPassantMove.startPos);
            break;
        case MoveType::PROMOTION:
            togglePiece(move.promotionMove.newPieceType, move.promotionMove.endPos);
            togglePiece(move.promotionMove.oldPieceType, move.promotionMove.startPos);
            togglePiece(move.promotionMove.killPieceType, move.promotionMove.endPos);
            break;
        case MoveType::NORMAL:
            togglePiece(move.normalMove.pieceType, move.normalMove.endPos);
            togglePiece(move.normalMove.pieceType, move.normalMove.startPos);
            togglePiece(move.normalMove.killPieceType, move.normalMove.endPos);
            break;
    }
}

/**
 * Used after completing a move, makes it so that there are no bit overflows after 64 full moves for castle data
 * Don't use if you need to go back a move for perft or minimax
 */
void Board::cleanup() {
    for (auto& rook : castleData)
        if (rook) rook = 0b1;
}

/**
 * Sets up the board in its starting position
 */
void Board::setDefaultBoard() {
    enPassantData = {};
    castleData = {};
    drawMoveCounter = 0;
    
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

    using namespace PieceType;
    mailBoxBoard = {
        WHITE_ROOK,   WHITE_PAWN, INVALID, INVALID, INVALID, INVALID, BLACK_PAWN, BLACK_ROOK,
        WHITE_KNIGHT, WHITE_PAWN, INVALID, INVALID, INVALID, INVALID, BLACK_PAWN, BLACK_KNIGHT,
        WHITE_BISHOP, WHITE_PAWN, INVALID, INVALID, INVALID, INVALID, BLACK_PAWN, BLACK_BISHOP,
        WHITE_QUEEN,  WHITE_PAWN, INVALID, INVALID, INVALID, INVALID, BLACK_PAWN, BLACK_QUEEN,
        WHITE_KING,   WHITE_PAWN, INVALID, INVALID, INVALID, INVALID, BLACK_PAWN, BLACK_KING,
        WHITE_BISHOP, WHITE_PAWN, INVALID, INVALID, INVALID, INVALID, BLACK_PAWN, BLACK_BISHOP,
        WHITE_KNIGHT, WHITE_PAWN, INVALID, INVALID, INVALID, INVALID, BLACK_PAWN, BLACK_KNIGHT,
        WHITE_ROOK,   WHITE_PAWN, INVALID, INVALID, INVALID, INVALID, BLACK_PAWN, BLACK_ROOK
    };
}
/**
 * Resets the board back to its initial/default state
 */
void Board::resetBoard() {
    castleData = {1, 1, 1, 1};
    enPassantData = {};
    bitBoards = {};
    for (int i = 0; i < 64; i++) mailBoxBoard[i] = PieceType::INVALID;
    drawMoveCounter = 0;
}

/**
 * Parses a given fen, storing relevent information
 * 
 * @param FEN the fen that is to be parsed
*/
void Board::parseFen(const std::string& FEN) {
    resetBoard();

    int i = 0;
    //parses the first part of the FEN
    for (int file = -1, rank = 7; i < FEN.length(); i++) {
        if (FEN[i] == ' ') break;
        
        size_t index = PieceType::enumOrder.find(FEN[i]);
        file++;

        if (FEN[i] == '/') {
            rank--;
            file = -1;
        }
        else if (index == -1) {
            file += FEN[i] - '1'; //'0'-1
        }
        else {
            addPiece((PieceType::Enum)index, (SquareIndex)(8*file+rank));
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
    if (FEN[i+1] != '-') {
        int index = (FEN[i+1] - 'a') + (FEN[i+2] =='3' ? 0 : 8);
        enPassantData[(EnPassantPieces)(index)] = 1;
        i--;
    }

    //parses the fifth part of the FEN
    for (i += 3; FEN[i] != ' '; i++) {
        drawMoveCounter = 10 *drawMoveCounter + (FEN[i]-'0');
    }
}

std::string Board::toFen() {
    std::string fen = "";
    
    int emptyCount = 0;
    for (int row = 7; row >= 0; row--) {
        for (int column = 0; column <= 7; column++) {
            int i = 8*column + row;
        
            //handle empty square
            if (mailBoxBoard[i] == PieceType::INVALID) {
                emptyCount++;
                continue;
            }

            //handle empty count
            if (emptyCount) {
                fen += std::to_string(emptyCount);
                emptyCount = 0;
            }

            //handle normal piece
            fen += PieceType::enumOrder[mailBoxBoard[i]];
        }

        //handle column row change
        if (emptyCount)
            fen += std::to_string(emptyCount);
        emptyCount = 0;
        fen += '/';
    }

    //add active colour data
    fen.back() = ' ';
    fen += whiteTurn ? 'w' : 'b';

    //add castle data
    fen += ' ';
    std::string rooks = "KQkq";
    for (int i = 0; i < castleData.size(); i++)
        if (!castleData[i]) fen += rooks[i];
    if (rooks.find(fen.back()) == -1) fen += '-';
    
    //add en passant data
    // fen += ' ';
    // for (int i = 0; i < enPassantData.size(); i++) {
        // if (enPassantData[i]&1) {
            // fen += (i%8)+'a';
            // fen += i < 8 ? '3' : '6';
        // }
    // }

    //add en passant data
    fen += ' ';
    for (const Move& m : MoveGeneration::generateMoves(*this)) {
        if (m.flag == EN_PASSANT) {
            fen += m.toString().substr(2, 2);
            break;
        }
    }
    if (fen.back() == ' ')
        fen += '-';

    //remove white space
    while (fen.back() == ' ') fen.pop_back();

    return fen;
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
void Board::addPiece(PieceType::Enum type, SquareIndex index) {
    if (type == PieceType::INVALID) return;
    bitBoards[type] |= (1ULL << index);
    bitBoards[PIECE_COLOUR(type) == PieceType::WHITE ? PieceType::WHITE_PIECES : PieceType::BLACK_PIECES] |= (1ULL << index);
    mailBoxBoard[index] = type;
}
//removes a piece to a given square
void Board::removePiece(PieceType::Enum type, SquareIndex index) {
    if (type == PieceType::INVALID) return;
    bitBoards[type] &= ~(1ULL << index);
    bitBoards[PIECE_COLOUR(type) == PieceType::WHITE ? PieceType::WHITE_PIECES : PieceType::BLACK_PIECES] &= ~(1ULL << index);
    mailBoxBoard[index] = PieceType::INVALID;
}
//toggles a piece in a given square
void Board::togglePiece(PieceType::Enum type, SquareIndex index) {
    if (type == PieceType::INVALID) return;
    bitBoards[type] ^= (1ULL << index);
    bitBoards[PIECE_COLOUR(type) == PieceType::WHITE ? PieceType::WHITE_PIECES : PieceType::BLACK_PIECES] ^= (1ULL << index);
    mailBoxBoard[index] = (mailBoxBoard[index] == PieceType::INVALID) ? type : PieceType::INVALID;
}

//prints the bitboard in binary
void Board::printBitBoardBin(PieceType::Enum board) {
    std::cout << std::bitset<64>(bitBoards[board]) << '\n';
}
//prints the bitboard in hex
void Board::printBitBoardHex(PieceType::Enum board) {
    printf("0x%016lx\n", bitBoards[board]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ HELPER METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//returns true if the given square has a piece
bool Board::hasPiece(SquareIndex index) const {
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
