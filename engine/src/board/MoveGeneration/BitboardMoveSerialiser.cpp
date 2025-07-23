#include "board/MoveGeneration/BitboardMoveSerialiser.hpp"

#include "board/BoardUtil.hpp"
#include "board/Move.hpp"
#include "board/Board.hpp"
#include "board/MoveGeneration/BitboardMoveGenerator.hpp"
#include <vector>

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

std::vector<Move> generateCastlingMovesWhite(Colour colourToPlay, const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData) {
    uint64_t movesBitboard = generateCastlingBitboardWhite(board, occupied, castleData);
    std::vector<Move> moves; return moves;
}
std::vector<Move> generateEnPassantMovesWhite(Colour colourToPlay, uint64_t friendlyPieces, std::array<__uint128_t, 16> enPassantData) {
    uint64_t movesBitboard = generateEnPassantBitboardWhite(friendlyPieces, enPassantData);
    std::vector<Move> moves; return moves;
}

