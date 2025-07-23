#include "board/MoveGeneration/BitboardMoveSerialiser.hpp"

#include <vector>

#include "board/BoardUtil.hpp"
#include "board/Move.hpp"
#include "board/Board.hpp"
#include "board/MoveGeneration/BitboardMoveGenerator.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ STATIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


static std::vector<Move> generateCastlingBitboardWhite(const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData);
static std::vector<Move> generateCastlingBitboardBlack(const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ PUBLIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<Move> generateKingMoves(Colour colourToPlay, uint64_t king, uint64_t friendlyPieces) {
    std::vector<Move> moves;
    moves.reserve(8);

    const SquareIndex   startSquare = (SquareIndex)__builtin_ctzll(king);
    const PieceType     pieceType = colourToPlay == Colour::WHITE ? WHITE_KING : BLACK_KING;

    uint64_t movesBitboard = generateKingBitboard(king, friendlyPieces);
    while (movesBitboard) {
        SquareIndex targetSquare = (SquareIndex)__builtin_ctzll(movesBitboard);

        moves.push_back({.flag=NORMAL, .normalMove=NormalMove{startSquare, targetSquare, pieceType}});

        movesBitboard &= movesBitboard-1;
    }

    return moves;
}
std::vector<Move> generateKnightMoves(Colour colourToPlay, uint64_t knights, uint64_t friendlyPieces) {
    std::vector<Move> moves;
    moves.reserve(8);

    return moves;
}

std::vector<Move> generatePawnMovesWhite(Colour colourToPlay, uint64_t pawns, uint64_t unoccupied) {
    std::vector<Move> moves; return moves;
}

std::vector<Move> generateRookMoves(Colour colourToPlay, uint64_t rooks, uint64_t occupied, uint64_t friendlyPieces) {
    std::vector<Move> moves; return moves;
}
std::vector<Move> generateBishopMoves(Colour colourToPlay, uint64_t bishops, uint64_t occupied, uint64_t friendlyPieces) {
    std::vector<Move> moves; return moves;
}
std::vector<Move> generateQueenMoves(Colour colourToPlay, uint64_t queens, uint64_t occupied, uint64_t friendlyPieces) {
    std::vector<Move> moves; return moves;
}

std::vector<Move> generateCastlingMoves(Colour colourToPlay, const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData) {
    if (colourToPlay == Colour::WHITE)
        return generateCastlingBitboardWhite(board, occupied, castleData);
    else
        return generateCastlingBitboardBlack(board, occupied, castleData);
}
std::vector<Move> generateEnPassantMoves(Colour colourToPlay, uint64_t friendlyPieces, std::array<__uint128_t, 16> enPassantData) {
    uint64_t movesBitboard = generateEnPassantBitboardWhite(friendlyPieces, enPassantData);
    std::vector<Move> moves; return moves;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ STATIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

static std::vector<Move> generateCastlingBitboardWhite(const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData) {
    std::vector<Move> moves;
    moves.reserve(2);

    if (castleData[CastlePieces::W_KING]  == 0 && (occupied & (uint64_t)(0x0001010000000000)) == 0) {
        if (!isTargeted(board, false, SquareIndex::f1) && !isTargeted(board, false, SquareIndex::g1)) {
            moves.push_back({.flag=CASTLE, .castleMove=CastleMove{e1, g1, WHITE_KING, h1, f1, WHITE_ROOK}});
        }
    }
    if (castleData[CastlePieces::W_QUEEN] == 0 && (occupied & (uint64_t)(0x0000000001010100)) == 0) {
        if (!isTargeted(board, false, SquareIndex::b1) && !isTargeted(board, false, SquareIndex::c1) && !isTargeted(board, false, SquareIndex::d1)) {
            moves.push_back({.flag=CASTLE, .castleMove=CastleMove{e1, c1, WHITE_KING, a1, d1, WHITE_ROOK}});
        }
    }

    return moves;
}
static std::vector<Move> generateCastlingBitboardBlack(const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData) {
    std::vector<Move> moves;
    moves.reserve(2);

    if (castleData[CastlePieces::B_KING]  == 0 && (occupied & (uint64_t)(0x0080800000000000)) == 0) {
        if (!isTargeted(board, false, SquareIndex::f8) && !isTargeted(board, false, SquareIndex::g8)) {
            moves.push_back({.flag=CASTLE, .castleMove=CastleMove{e8, g8, WHITE_KING, h8, f8, WHITE_ROOK}});
        }
    }
    if (castleData[CastlePieces::B_QUEEN] == 0 && (occupied & (uint64_t)(0x0000000080808000)) == 0) {
        if (!isTargeted(board, false, SquareIndex::b8) && !isTargeted(board, false, SquareIndex::c8) && !isTargeted(board, false, SquareIndex::d8)) {
            moves.push_back({.flag=CASTLE, .castleMove=CastleMove{e8, c8, WHITE_KING, a8, d8, WHITE_ROOK}});
        }
    }

    return moves;
}

