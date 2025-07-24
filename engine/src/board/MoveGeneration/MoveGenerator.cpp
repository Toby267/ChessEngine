#include "board/MoveGeneration/MoveGenerator.hpp"

#include <iostream>
#include <vector>
#include <cstdint>

#include "board/MoveGeneration/BitboardMoveSerialiser.hpp"
#include "board/MoveGeneration/BitboardMoveGenerator.hpp"
#include "board/Board.hpp"
#include "board/BoardUtil.hpp"

/**
 * Generates all possible moves based on a given board and whos to move
 * 
 * @param board the board
 * @param whiteTurn whether or not it is whites turn to move
 */
std::vector<Move> generateMoves(Board& board, WhiteTurn whiteTurn) {//todo: remove the temp variable
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

    //generate moves for the easy pieces
    uint64_t kingMoves      = generateKingBitboard(bitBoards[PieceType::WHITE_KING + indexOffset], friendlyPieces);
    uint64_t knightMoves    = generateKnightBitboard(bitBoards[PieceType::WHITE_KNIGHT + indexOffset], friendlyPieces);

    //generate moves for pawns
    uint64_t pawnPushes     = generatePawnPushBitboard(whiteTurn, bitBoards[PieceType::WHITE_PAWN + indexOffset], unoccupied);
    uint64_t pawnAttacks    = generatePawnAttackBitboard(whiteTurn, bitBoards[PieceType::WHITE_PAWN + indexOffset], oppositionPieces);

    //generate moves for sliding pieces
    uint64_t rookMoves      = generateRookBitboard(bitBoards[PieceType::WHITE_ROOK + indexOffset], occupied, friendlyPieces);
    uint64_t bishopMoves    = generateBishopBitboard(bitBoards[PieceType::WHITE_BISHOP + indexOffset], occupied, friendlyPieces);
    uint64_t queenMoves     = generateQueenBitboard(bitBoards[PieceType::WHITE_QUEEN + indexOffset], occupied, friendlyPieces);

    //generate en passant and castling
    // uint64_t castleMoves    = whiteTurn ?   generateCastlingBitboardWhite(board, occupied, castleData):
                                            // generateCastlingBitboardBlack(board, occupied, castleData);
    // uint64_t enPassantMoves = whiteTurn ?   generateEnPassantBitboardWhite(bitBoards[PieceType::WHITE_PAWN], enPassantData):
                                            // generateEnPassantBitboardBlack(bitBoards[PieceType::BLACK_PAWN], enPassantData);

    //serialise into moves vector
    moves = generateCastlingMoves(whiteTurn, board, occupied, castleData);
    for (auto& i : moves) {
        std::cout << i.flag                             << '\n';
        std::cout << i.castleMove.primaryStartPos       << '\n';
        std::cout << i.castleMove.primaryEndPos         << '\n';
        std::cout << i.castleMove.primaryPieceType      << '\n';
        std::cout << i.castleMove.secondaryStartPos     << '\n';
        std::cout << i.castleMove.secondaryEndPos       << '\n';
        std::cout << i.castleMove.secondaryPieceType    << '\n';
    }

    // board.resetBoard();
    // board.setBitBoard(castleMoves, PieceType::BLACK_KING);
    // board.setBitBoard(castleMoves, PieceType::BLACK_PIECES);

    //finally return
    return moves;
}
