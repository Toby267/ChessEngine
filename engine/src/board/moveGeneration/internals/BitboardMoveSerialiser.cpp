#include "board/moveGeneration/internals/MoveGeneratorInternals.hpp"

#include <cstdint>
#include <vector>

#include "board/BoardUtil.hpp"
#include "board/Move.hpp"
#include "board/Board.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ STATIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void addCastlingMovesWhite(std::vector<Move>& moves, const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData);
static void addCastlingMovesBlack(std::vector<Move>& moves, const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData);

static void addPawnPushMovesWhite(std::vector<Move>& moves, const Board& board, uint64_t pawns, uint64_t unoccupied);
static void addPawnPushMovesBlack(std::vector<Move>& moves, const Board& board, uint64_t pawns, uint64_t unoccupied);

static void addPawnAttackMovesWhite(std::vector<Move>& moves, const Board& board, uint64_t pawns, uint64_t oppositionPieces);
static void addPawnAttackMovesBlack(std::vector<Move>& moves, const Board& board, uint64_t pawns, uint64_t oppositionPieces);

static void addSinglePawnMoveWhite(std::vector<Move>& moves, const Board& board, SquareIndex startPos, SquareIndex endPos);
static void addSinglePawnMoveBlack(std::vector<Move>& moves, const Board& board, SquareIndex startPos, SquareIndex endPos);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ PUBLIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// * ------------------------------------------- [ EASY MOVES ] ------------------------------------------ * //

//generates and adds all king moves to the moves reference
void generateKingMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t king, uint64_t friendlyPieces) {
    const SquareIndex   startSquare = (SquareIndex)__builtin_ctzll(king);
    const PieceType::Enum     pieceType = whiteTurn ? PieceType::WHITE_KING : PieceType::BLACK_KING;

    uint64_t movesBitboard = generateKingBitboard(king, friendlyPieces);
    while (movesBitboard) {
        SquareIndex targetSquare = (SquareIndex)__builtin_ctzll(movesBitboard);

        //TODO: this method creates a temporary object for move and copies it, need to find a way to use emplace_back
        moves.emplace_back(NORMAL, NormalMove{startSquare, targetSquare, pieceType, board.getType(targetSquare)});

        movesBitboard &= movesBitboard-1;
    }
}
//generates and adds all knight moves to the moves reference
void generateKnightMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t knights, uint64_t friendlyPieces) {
    const PieceType::Enum pieceType = whiteTurn ? PieceType::WHITE_KNIGHT : PieceType::BLACK_KNIGHT;

    while (knights) {
        SquareIndex startSquare = (SquareIndex)__builtin_ctzll(knights);

        uint64_t movesBitboard = generateKnightBitboardSingular(startSquare, friendlyPieces);
        while (movesBitboard) {
            SquareIndex targetSquare = (SquareIndex)__builtin_ctzll(movesBitboard);

            moves.emplace_back(NORMAL, NormalMove{startSquare, targetSquare, pieceType, board.getType(targetSquare)});

            movesBitboard &= movesBitboard-1;
        }

        knights &= knights-1;
    }
}

// * ------------------------------------------ [ SLIDING MOVES ] ---------------------------------------- * //

//generates and adds all rook moves to the moves reference
void generateRookMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t rooks, uint64_t occupied, uint64_t friendlyPieces) {
    const PieceType::Enum pieceType = whiteTurn ? PieceType::WHITE_ROOK : PieceType::BLACK_ROOK;

    while (rooks) {
        SquareIndex startSquare = (SquareIndex)__builtin_ctzll(rooks);

        uint64_t movesBitboard = generateRookBitboardSingular(startSquare, occupied, friendlyPieces);
        while (movesBitboard) {
            SquareIndex targetSquare = (SquareIndex)__builtin_ctzll(movesBitboard);

            moves.emplace_back(NORMAL, NormalMove{startSquare, targetSquare, pieceType, board.getType(targetSquare)});

            movesBitboard &= movesBitboard-1;
        }

        rooks &= rooks-1;
    }
}
//generates and adds all bishop moves to the moves reference
void generateBishopMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t bishops, uint64_t occupied, uint64_t friendlyPieces) {
    const PieceType::Enum pieceType = whiteTurn ? PieceType::WHITE_BISHOP : PieceType::BLACK_BISHOP;

    while (bishops) {
        SquareIndex startSquare = (SquareIndex)__builtin_ctzll(bishops);

        uint64_t movesBitboard = generateBishopBitboardSingular(startSquare, occupied, friendlyPieces);
        while (movesBitboard) {
            SquareIndex targetSquare = (SquareIndex)__builtin_ctzll(movesBitboard);

            moves.emplace_back(NORMAL, NormalMove{startSquare, targetSquare, pieceType, board.getType(targetSquare)});

            movesBitboard &= movesBitboard-1;
        }

        bishops &= bishops-1;
    }
}
//generates and adds all queen moves to the moves reference
void generateQueenMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t queens, uint64_t occupied, uint64_t friendlyPieces) {
    const PieceType::Enum pieceType = whiteTurn ? PieceType::WHITE_QUEEN : PieceType::BLACK_QUEEN;

    while (queens) {
        SquareIndex startSquare = (SquareIndex)__builtin_ctzll(queens);

        uint64_t movesBitboard = generateQueenBitboardSingular(startSquare, occupied, friendlyPieces);
        while (movesBitboard) {
            SquareIndex targetSquare = (SquareIndex)__builtin_ctzll(movesBitboard);

            moves.emplace_back(NORMAL, NormalMove{startSquare, targetSquare, pieceType, board.getType(targetSquare)});

            movesBitboard &= movesBitboard-1;
        }

        queens &= queens-1;
    }
}

// * -------------------------------------- [ PAWN & SPECIAL MOVES ] ------------------------------------- * //

//generates and adds all pawn moves to the moves reference
void generatePawnMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t pawns, uint64_t unoccupied, uint64_t oppositionPieces) {
    if (whiteTurn) {
        addPawnPushMovesWhite(moves, board, pawns, unoccupied);
        addPawnAttackMovesWhite(moves, board, pawns, oppositionPieces);
    }
    else {
        addPawnPushMovesBlack(moves, board, pawns, unoccupied);
        addPawnAttackMovesBlack(moves, board, pawns, oppositionPieces);
    }
}
//generates and adds all castling moves to the moves reference
void generateCastlingMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t occupied, std::array<__uint128_t, 4> castleData) {
    if (whiteTurn)
        addCastlingMovesWhite(moves, board, occupied, castleData);
    else
        addCastlingMovesBlack(moves, board, occupied, castleData);
}
//generates and adds all en passant moves to the moves reference
void generateEnPassantMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t pawns, std::array<__uint128_t, 16> enPassantData) {
    //very cursed line. I could of just put the ternary in the two lines that this is used as its just as efficient time wise and more efficient space wise but this looks cooler
    SquareIndex (*forwardOne)(int) = whiteTurn ? (SquareIndex(*)(int))northOne : (SquareIndex(*)(int))southOne;
    PieceType::Enum pieceType = whiteTurn ? PieceType::WHITE_PAWN : PieceType::BLACK_PAWN;
    PieceType::Enum killPieceType = whiteTurn ? PieceType::BLACK_PAWN : PieceType::WHITE_PAWN;

    for (int i = 0; i < enPassantData.size(); i++) {
        if (!(enPassantData[i] & 1)) continue;

        int killIndex = ((i % 8) * 8) + ((i > 7) ? 4 : 3);
        uint64_t pawnBitboard = 1ULL << killIndex;

        if (pawnBitboard & pawns) return;

        if (westOne(pawnBitboard) & pawns) {
            moves.emplace_back(EN_PASSANT, EnPassantMove{westOne(killIndex), forwardOne(killIndex), pieceType, (SquareIndex)killIndex, killPieceType});
        }
        if (eastOne(pawnBitboard) & pawns) {
            moves.emplace_back(EN_PASSANT, EnPassantMove{eastOne(killIndex), forwardOne(killIndex), pieceType, (SquareIndex)killIndex, killPieceType});
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ STATIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// * ------------------------------------------ [ CASTLING MOVES ] --------------------------------------- * //

//generates and adds all white castling moves to the moves reference
static void addCastlingMovesWhite(std::vector<Move>& moves, const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData) {
    if (castleData[CastlePieces::W_KING]  == 0 && (occupied & (uint64_t)(0x0001010000000000)) == 0) {
        if (!isTargeted(board, WhiteTurn{false}, SquareIndex::e1) && !isTargeted(board, WhiteTurn{false}, SquareIndex::f1)) {
            moves.emplace_back(CASTLE, CastleMove{e1, g1, PieceType::WHITE_KING, h1, f1, PieceType::WHITE_ROOK});
        }
    }
    if (castleData[CastlePieces::W_QUEEN] == 0 && (occupied & (uint64_t)(0x0000000001010100)) == 0) {
        if (!isTargeted(board, WhiteTurn{false}, SquareIndex::e1) && !isTargeted(board, WhiteTurn{false}, SquareIndex::d1)) {
            moves.emplace_back(CASTLE, CastleMove{e1, c1, PieceType::WHITE_KING, a1, d1, PieceType::WHITE_ROOK});
        }
    }
}
//generates and adds all black castling moves to the moves reference
static void addCastlingMovesBlack(std::vector<Move>& moves, const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData) {
    if (!castleData[CastlePieces::B_KING] && !(occupied & (uint64_t)(0x0080800000000000))) {
        if (!isTargeted(board, WhiteTurn{true}, SquareIndex::e8) && !isTargeted(board, WhiteTurn{true}, SquareIndex::f8)) {
            moves.emplace_back(CASTLE, CastleMove{e8, g8, PieceType::BLACK_KING, h8, f8, PieceType::BLACK_ROOK});
        }
    }
    if (!castleData[CastlePieces::B_QUEEN] && !(occupied & (uint64_t)(0x0000000080808000))) {
        if (!isTargeted(board, WhiteTurn{true}, SquareIndex::e8) && !isTargeted(board, WhiteTurn{true}, SquareIndex::d8)) {
            moves.emplace_back(CASTLE, CastleMove{e8, c8, PieceType::BLACK_KING, a8, d8, PieceType::BLACK_ROOK});
        }
    }
}

// * ------------------------------------------- [ EASY MOVES ] ------------------------------------------ * //

//generates and adds all white pawn push moves to the moves reference
static void addPawnPushMovesWhite(std::vector<Move>& moves, const Board& board, uint64_t pawns, uint64_t unoccupied) {
    uint64_t pushMoves = generatePawnPushBitboard(WhiteTurn{true}, pawns, unoccupied);

    while (pushMoves) {
        SquareIndex targetSquareIndex = (SquareIndex)__builtin_ctzll(pushMoves);
        uint64_t targetSquare = 1ULL << targetSquareIndex;

        //add single push move
        addSinglePawnMoveWhite(moves, board, southOne(targetSquareIndex), targetSquareIndex);

        pushMoves &= pushMoves-1;

        if (northOne(targetSquare) & pushMoves) {
            //add double push move
            addSinglePawnMoveWhite(moves, board, southOne(targetSquareIndex), northOne(targetSquareIndex));

            pushMoves &= pushMoves-1;
        }
    }
}
//generates and adds all white pawn attack moves to the moves reference
static void addPawnAttackMovesWhite(std::vector<Move>& moves, const Board& board, uint64_t pawns, uint64_t oppositionPieces) {
    uint64_t attackMoves = generatePawnAttackBitboard(WhiteTurn{true}, pawns, oppositionPieces);

    while (attackMoves) {
        SquareIndex targetSquareIndex = (SquareIndex)__builtin_ctzll(attackMoves);
        uint64_t targetSquare = 1ULL << targetSquareIndex;

        if (southEastOne(targetSquare) & pawns) {
            addSinglePawnMoveWhite(moves, board, southEastOne(targetSquareIndex), targetSquareIndex);
        }
        if (southWestOne(targetSquare) & pawns) {
            addSinglePawnMoveWhite(moves, board, southWestOne(targetSquareIndex), targetSquareIndex);
        }

        attackMoves &= attackMoves-1;
    }
}
//adds a single white pawn move to the moves reference
static void addSinglePawnMoveWhite(std::vector<Move>& moves, const Board& board, SquareIndex startPos, SquareIndex endPos) {
    if ((1ULL << endPos) & 0x8080808080808080) {
        //promotion moves
        moves.emplace_back(PROMOTION, PromotionMove{startPos, endPos, PieceType::WHITE_PAWN, PieceType::WHITE_QUEEN,  board.getType(endPos)});
        moves.emplace_back(PROMOTION, PromotionMove{startPos, endPos, PieceType::WHITE_PAWN, PieceType::WHITE_BISHOP, board.getType(endPos)});
        moves.emplace_back(PROMOTION, PromotionMove{startPos, endPos, PieceType::WHITE_PAWN, PieceType::WHITE_KNIGHT, board.getType(endPos)});
        moves.emplace_back(PROMOTION, PromotionMove{startPos, endPos, PieceType::WHITE_PAWN, PieceType::WHITE_ROOK,   board.getType(endPos)});
    }
    else {
        //normal move
        moves.emplace_back(NORMAL, NormalMove{startPos, endPos, PieceType::WHITE_PAWN, board.getType(endPos)});
    }
}

//generates and adds all black pawn push moves to the moves reference
static void addPawnPushMovesBlack(std::vector<Move>& moves, const Board& board, uint64_t pawns, uint64_t unoccupied) {
    uint64_t pushMoves = generatePawnPushBitboard(WhiteTurn{false}, pawns, unoccupied);

    while (pushMoves) {
        SquareIndex targetSquareIndex = (SquareIndex)__builtin_ctzll(pushMoves);
        uint64_t targetSquare = 1ULL << targetSquareIndex;

        if (northOne(targetSquare) & pushMoves) {
            //add double and single push move
            addSinglePawnMoveBlack(moves, board, northOne(northOne(targetSquareIndex)), targetSquareIndex);
            addSinglePawnMoveBlack(moves, board, northOne(northOne(targetSquareIndex)), northOne(targetSquareIndex));

            pushMoves &= pushMoves-1;
        }
        else {
            //add single push move
            addSinglePawnMoveBlack(moves, board, northOne(targetSquareIndex), targetSquareIndex);
        }

        pushMoves &= pushMoves-1;
    }
}
//generates and adds all black pawn attack moves to the moves reference
static void addPawnAttackMovesBlack(std::vector<Move>& moves, const Board& board, uint64_t pawns, uint64_t oppositionPieces) {
    uint64_t attackMoves = generatePawnAttackBitboard(WhiteTurn{false}, pawns, oppositionPieces);
    
    while (attackMoves) {
        SquareIndex targetSquareIndex = (SquareIndex)__builtin_ctzll(attackMoves);
        uint64_t targetSquare = 1ULL << targetSquareIndex;
        
        if (northEastOne(targetSquare) & pawns) {
            addSinglePawnMoveBlack(moves, board, northEastOne(targetSquareIndex), targetSquareIndex);
        }
        if (northWestOne(targetSquare) & pawns) {
            addSinglePawnMoveBlack(moves, board, northWestOne(targetSquareIndex), targetSquareIndex);
        }

        attackMoves &= attackMoves-1;
    }
}
//adds a single black pawn move to the moves reference
static void addSinglePawnMoveBlack(std::vector<Move>& moves, const Board& board, SquareIndex startPos, SquareIndex endPos) {
    if ((1ULL << endPos) & 0x0101010101010101) {
        //promotion moves
        moves.emplace_back(PROMOTION, PromotionMove{startPos, endPos, PieceType::BLACK_PAWN, PieceType::BLACK_QUEEN,  board.getType(endPos)});
        moves.emplace_back(PROMOTION, PromotionMove{startPos, endPos, PieceType::BLACK_PAWN, PieceType::BLACK_BISHOP, board.getType(endPos)});
        moves.emplace_back(PROMOTION, PromotionMove{startPos, endPos, PieceType::BLACK_PAWN, PieceType::BLACK_KNIGHT, board.getType(endPos)});
        moves.emplace_back(PROMOTION, PromotionMove{startPos, endPos, PieceType::BLACK_PAWN, PieceType::BLACK_ROOK,   board.getType(endPos)});
    }
    else {
        //normal move
        moves.emplace_back(NORMAL, NormalMove{startPos, endPos, PieceType::BLACK_PAWN, board.getType(endPos)});
    }
}
