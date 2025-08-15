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
 * @return a vector of valid moves
 */
std::vector<Move> MoveGeneration::generateMoves(Board& board) {
    //return vector containing all moves converted into type Move.
    std::vector<Move> moves;
    moves.reserve(32);

    //constant values including the bitboards and masks
    const std::array<uint64_t, 14>&     bitBoards           = board.getBitBoards();
    const std::array<__uint128_t, 16>&  enPassantData       = board.getEnPassantData();
    const std::array<__uint128_t, 4>&   castleData          = board.getCastleData();
    const WhiteTurn                     whiteTurn           = board.getWhiteTurn();
    const uint64_t                      whitePieces         = bitBoards[PieceType::WHITE_PIECES];
    const uint64_t                      blackPieces         = bitBoards[PieceType::BLACK_PIECES];
    const uint64_t                      friendlyPieces      = whiteTurn ? whitePieces : blackPieces;
    const uint64_t                      oppositionPieces    = whiteTurn ? blackPieces : whitePieces;
    const uint64_t                      occupied            = whitePieces | blackPieces;
    const uint64_t                      unoccupied          = ~occupied;
    const short                         indexOffset         = whiteTurn ? 0 : PieceType::BLACK-PieceType::WHITE;
    
    //generate moves
    generateEnPassantMoves(moves, board, whiteTurn, bitBoards[PieceType::WHITE_PAWN + indexOffset], enPassantData);
    generateCastlingMoves(moves, board, whiteTurn, occupied, castleData);
    generateKnightMoves(moves, board, whiteTurn, bitBoards[PieceType::WHITE_KNIGHT + indexOffset], friendlyPieces);
    generatePawnMoves(moves, board, whiteTurn, bitBoards[PieceType::WHITE_PAWN + indexOffset], unoccupied, oppositionPieces);
    generateBishopMoves(moves, board, whiteTurn, bitBoards[PieceType::WHITE_BISHOP + indexOffset], occupied, friendlyPieces);
    generateRookMoves(moves, board, whiteTurn, bitBoards[PieceType::WHITE_ROOK + indexOffset], occupied, friendlyPieces);
    generateQueenMoves(moves, board, whiteTurn, bitBoards[PieceType::WHITE_QUEEN + indexOffset], occupied, friendlyPieces);
    generateKingMoves(moves, board, whiteTurn, bitBoards[PieceType::WHITE_KING + indexOffset], friendlyPieces);

    //filter out moves that leave the king in check
    std::vector<Move> retMoves;
    retMoves.reserve(moves.size());

    SquareIndex kingIndex;

    for (auto& move : moves) {
        board.makeMove(move);
        kingIndex = (SquareIndex)__builtin_ctzll(bitBoards[PieceType::WHITE_KING + indexOffset]);

        if (!isTargeted(board, !whiteTurn, kingIndex))
            retMoves.emplace_back(move);

        board.unMakeMove(move);
    }

    //finally return
    return retMoves;
}

/**
 * Determines whether the king is currently being targetted in the given board
 * 
 * @param board the board
 * @return whether or not the king is being targeted
 */
bool MoveGeneration::isKingTargeted(const Board& board) {
    const std::array<uint64_t, 14>& bitBoards       = board.getBitBoards();
    const WhiteTurn                 whiteTurn       = board.getWhiteTurn();
    const uint64_t                  kingBitboard    = whiteTurn ? bitBoards[PieceType::WHITE_KING] : bitBoards[PieceType::BLACK_KING];
    const SquareIndex               kingIndex       = (SquareIndex)__builtin_ctzll(kingBitboard);
    
    return isTargeted(board, !whiteTurn, kingIndex);
}
