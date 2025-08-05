#include "board/moveGeneration/internals/MoveGeneratorInternals.hpp"

/**
 * Returns if a specific square is being targeted by any pseduo legal move in a given position 
 * 
 * @param board the board
 * @param whiteTurn whether or not it is whites turn to move
 * @param i the given square index
 * @return whether or not the given index is being targeted
 */
bool isTargeted(const Board& board, WhiteTurn whiteTurn, SquareIndex i) {
    //targeted piece
    uint64_t targetedPiece = 1ULL << i;
    
    //constant values including the bitboards and masks
    const std::array<uint64_t, 14>&     bitBoards           = board.getBitBoards();
    const std::array<__uint128_t, 16>&  enPassantData       = board.getEnPassantData();
    const uint64_t                      whitePieces         = bitBoards[PieceType::WHITE_PIECES];
    const uint64_t                      blackPieces         = bitBoards[PieceType::BLACK_PIECES];
    const uint64_t                      friendlyPieces      = whiteTurn ? whitePieces : blackPieces;
    const uint64_t                      oppositionPieces    = whiteTurn ? blackPieces : whitePieces;
    const uint64_t                      occupied            = whitePieces | blackPieces;
    const short                         indexOffset         = whiteTurn ? 0 : 6;

    //generate bitboards for the king and knights
    uint64_t kingMoves      = generateKingBitboard(bitBoards[PieceType::WHITE_KING + indexOffset], friendlyPieces);
    uint64_t knightMoves    = generateKnightBitboard(bitBoards[PieceType::WHITE_KNIGHT + indexOffset], friendlyPieces);
    if (targetedPiece & kingMoves || targetedPiece & knightMoves) return true;

    //generate bitboards for sliding pieces
    uint64_t rookMoves      = generateRookBitboard(bitBoards[PieceType::WHITE_ROOK + indexOffset], occupied, friendlyPieces);
    uint64_t bishopMoves    = generateBishopBitboard(bitBoards[PieceType::WHITE_BISHOP + indexOffset], occupied, friendlyPieces);
    uint64_t queenMoves     = generateQueenBitboard(bitBoards[PieceType::WHITE_QUEEN + indexOffset], occupied, friendlyPieces);
    if (targetedPiece & rookMoves || targetedPiece & bishopMoves || targetedPiece & queenMoves) return true;

    //generate bitboards for pawns
    uint64_t pawnAttacks    = generatePawnTargetBitboard(whiteTurn, bitBoards[PieceType::WHITE_PAWN + indexOffset]);
    uint64_t enPassantMoves = generateEnPassantBitboard(whiteTurn, bitBoards[PieceType::WHITE_PAWN + indexOffset], enPassantData);
    if (targetedPiece & pawnAttacks || targetedPiece & enPassantMoves) return true;

    return false;
}
