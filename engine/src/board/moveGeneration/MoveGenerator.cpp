#include "board/moveGeneration/MoveGenerator.hpp"

#include <vector>
#include <cstdint>

#include "board/Move.hpp"
#include "board/Board.hpp"
#include "board/BoardUtil.hpp"
#include "board/moveGeneration/internals/MoveGeneratorInternals.hpp"

/**
 * Generates all possible moves based on a given board and whos to move
 * 
 * @param board the board
 * @param whiteTurn whether or not it is whites turn to move
 */
std::vector<Move> generateMoves(Board& board, WhiteTurn whiteTurn) {
    //return vector containing all moves converted into type Move.
    std::vector<Move> moves;
    moves.reserve(32);

    //constant values including the bitboards and masks
    const std::array<uint64_t, 14>&     bitBoards           = board.getBitBoards();
    const std::array<__uint128_t, 16>&  enPassantData       = board.getEnPassantData();
    const std::array<__uint128_t, 4>&   castleData          = board.getCastleData();
    const uint64_t                      whitePieces         = bitBoards[PieceType::WHITE_PIECES];
    const uint64_t                      blackPieces         = bitBoards[PieceType::BLACK_PIECES];
    const uint64_t                      friendlyPieces      = whiteTurn ? whitePieces : blackPieces;
    const uint64_t                      oppositionPieces    = whiteTurn ? blackPieces : whitePieces;
    const uint64_t                      occupied            = whitePieces | blackPieces;
    const uint64_t                      unoccupied          = ~occupied;
    const short                         indexOffset         = whiteTurn ? 0 : 6;

    //generate moves
    generateKingMoves(moves, board, whiteTurn, bitBoards[PieceType::WHITE_KING + indexOffset], friendlyPieces);
    generateKnightMoves(moves, board, whiteTurn, bitBoards[PieceType::WHITE_KNIGHT + indexOffset], friendlyPieces);
    generateRookMoves(moves, board, whiteTurn, bitBoards[PieceType::WHITE_ROOK + indexOffset], occupied, friendlyPieces);
    generateBishopMoves(moves, board, whiteTurn, bitBoards[PieceType::WHITE_BISHOP + indexOffset], occupied, friendlyPieces);
    generateQueenMoves(moves, board, whiteTurn, bitBoards[PieceType::WHITE_QUEEN + indexOffset], occupied, friendlyPieces);
    generatePawnMoves(moves, board, whiteTurn, bitBoards[PieceType::WHITE_PAWN + indexOffset], unoccupied, oppositionPieces);
    generateCastlingMoves(moves, board, whiteTurn, occupied, castleData);
    generateEnPassantMoves(moves, board, whiteTurn, friendlyPieces, enPassantData);

    //filter out moves that leave the king in check
    // for (int i = 0; i < moves.size(); i++) {
    //     board.makeMove(moves[i]);
    // }

    //finally return
    return moves;
}
