#include "board/moveGeneration/MoveGeneratorInternals.hpp"

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

//generates a vector of all king moves
void generateKingMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t king, uint64_t friendlyPieces) {
    const SquareIndex   startSquare = (SquareIndex)__builtin_ctzll(king);
    const PieceType     pieceType = whiteTurn ? WHITE_KING : BLACK_KING;

    uint64_t movesBitboard = generateKingBitboard(king, friendlyPieces);
    while (movesBitboard) {
        SquareIndex targetSquare = (SquareIndex)__builtin_ctzll(movesBitboard);

        //TODO: this method creates a temporary object for move and copies it, need to find a way to use emplace_back
        moves.push_back({.flag=NORMAL, .normalMove=NormalMove{startSquare, targetSquare, pieceType, board.getType(targetSquare)}});

        movesBitboard &= movesBitboard-1;
    }
}

//generates a vector of all knight moves
void generateKnightMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t knights, uint64_t friendlyPieces) {
    const PieceType pieceType = whiteTurn ? WHITE_KNIGHT : BLACK_KNIGHT;

    while (knights) {
        SquareIndex startSquare = (SquareIndex)__builtin_ctzll(knights);

        uint64_t movesBitboard = generateKnightBitboardSingular(startSquare, friendlyPieces);
        while (movesBitboard) {
            SquareIndex targetSquare = (SquareIndex)__builtin_ctzll(movesBitboard);

            moves.push_back({.flag=NORMAL, .normalMove=NormalMove{startSquare, targetSquare, pieceType, board.getType(targetSquare)}});

            movesBitboard &= movesBitboard-1;
        }

        knights &= knights-1;
    }
}

//generates a vector of all rook moves
void generateRookMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t rooks, uint64_t occupied, uint64_t friendlyPieces) {
    const PieceType pieceType = whiteTurn ? WHITE_ROOK : BLACK_ROOK;

    while (rooks) {
        SquareIndex startSquare = (SquareIndex)__builtin_ctzll(rooks);

        uint64_t movesBitboard = generateRookBitboardSingular(startSquare, occupied, friendlyPieces);
        while (movesBitboard) {
            SquareIndex targetSquare = (SquareIndex)__builtin_ctzll(movesBitboard);

            moves.push_back({.flag=NORMAL, .normalMove=NormalMove{startSquare, targetSquare, pieceType, board.getType(targetSquare)}});

            movesBitboard &= movesBitboard-1;
        }

        rooks &= rooks-1;
    }
}

//generates a vector of all bishop moves
void generateBishopMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t bishops, uint64_t occupied, uint64_t friendlyPieces) {
    const PieceType pieceType = whiteTurn ? WHITE_BISHOP : BLACK_BISHOP;

    while (bishops) {
        SquareIndex startSquare = (SquareIndex)__builtin_ctzll(bishops);

        uint64_t movesBitboard = generateBishopBitboardSingular(startSquare, occupied, friendlyPieces);
        while (movesBitboard) {
            SquareIndex targetSquare = (SquareIndex)__builtin_ctzll(movesBitboard);

            moves.push_back({.flag=NORMAL, .normalMove=NormalMove{startSquare, targetSquare, pieceType, board.getType(targetSquare)}});

            movesBitboard &= movesBitboard-1;
        }

        bishops &= bishops-1;
    }
}

//generates a vector of all queen moves
void generateQueenMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t queens, uint64_t occupied, uint64_t friendlyPieces) {
    const PieceType pieceType = whiteTurn ? WHITE_QUEEN : BLACK_QUEEN;

    while (queens) {
        SquareIndex startSquare = (SquareIndex)__builtin_ctzll(queens);

        uint64_t movesBitboard = generateQueenBitboardSingular(startSquare, occupied, friendlyPieces);
        while (movesBitboard) {
            SquareIndex targetSquare = (SquareIndex)__builtin_ctzll(movesBitboard);

            moves.push_back({.flag=NORMAL, .normalMove=NormalMove{startSquare, targetSquare, pieceType, board.getType(targetSquare)}});

            movesBitboard &= movesBitboard-1;
        }

        queens &= queens-1;
    }
}

//generates a vector of all pawn moves
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

//generates a vector of all castling moves
void generateCastlingMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t occupied, std::array<__uint128_t, 4> castleData) {
    if (whiteTurn)
        addCastlingMovesWhite(moves, board, occupied, castleData);
    else
        addCastlingMovesBlack(moves, board, occupied, castleData);
}

//generates a vector of all en passant moves
void generateEnPassantMoves(std::vector<Move>& moves, const Board& board, WhiteTurn whiteTurn, uint64_t friendlyPieces, std::array<__uint128_t, 16> enPassantData) {
    //very cursed line. I could of just put the ternary in the two lines that this is used as its just as efficient time wise and more efficient space wise but this looks cooler
    SquareIndex (*forwardOne)(int) = whiteTurn ? (SquareIndex(*)(int))northOne : (SquareIndex(*)(int))southOne;

    for (int i = 0; i < enPassantData.size(); i++) {
        if (!(enPassantData[i] & 1)) continue;

        int killIndex = ((i % 8) * 8) + ((i > 7) ? 4 : 3);
        uint64_t pawnBitboard = 1ULL << killIndex;

        if (pawnBitboard & friendlyPieces) continue;

        if (westOne(pawnBitboard) & friendlyPieces) {
            moves.push_back({.flag=EN_PASSANT, .enPassantMove=EnPassantMove{westOne(killIndex), forwardOne(killIndex), WHITE_PAWN, (SquareIndex)killIndex, BLACK_PAWN}});
        }
        else if (eastOne(pawnBitboard) & friendlyPieces) {
            moves.push_back({.flag=EN_PASSANT, .enPassantMove=EnPassantMove{eastOne(killIndex), forwardOne(killIndex), WHITE_PAWN, (SquareIndex)killIndex, BLACK_PAWN}});
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ STATIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//generates a vector of all castling moves for the white pieces
static void addCastlingMovesWhite(std::vector<Move>& moves, const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData) {
    if (castleData[CastlePieces::W_KING]  == 0 && (occupied & (uint64_t)(0x0001010000000000)) == 0) {
        if (!isTargeted(board, WhiteTurn{false}, SquareIndex::f1) && !isTargeted(board, WhiteTurn{false}, SquareIndex::g1)) {
            moves.push_back({.flag=CASTLE, .castleMove=CastleMove{e1, g1, WHITE_KING, h1, f1, WHITE_ROOK}});
        }
    }
    if (castleData[CastlePieces::W_QUEEN] == 0 && (occupied & (uint64_t)(0x0000000001010100)) == 0) {
        if (!isTargeted(board, WhiteTurn{false}, SquareIndex::b1) && !isTargeted(board, WhiteTurn{false}, SquareIndex::c1) && !isTargeted(board, WhiteTurn{false}, SquareIndex::d1)) {
            moves.push_back({.flag=CASTLE, .castleMove=CastleMove{e1, c1, WHITE_KING, a1, d1, WHITE_ROOK}});
        }
    }
}
//generates a vector of all castling moves for the black pieces
static void addCastlingMovesBlack(std::vector<Move>& moves, const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData) {
    if (!castleData[CastlePieces::B_KING] && !(occupied & (uint64_t)(0x0080800000000000))) {
        if (!isTargeted(board, WhiteTurn{false}, SquareIndex::f8) && !isTargeted(board, WhiteTurn{false}, SquareIndex::g8)) {
            moves.push_back({.flag=CASTLE, .castleMove=CastleMove{e8, g8, WHITE_KING, h8, f8, WHITE_ROOK}});
        }
    }
    if (!castleData[CastlePieces::B_QUEEN] && !(occupied & (uint64_t)(0x0000000080808000))) {
        if (!isTargeted(board, WhiteTurn{false}, SquareIndex::b8) && !isTargeted(board, WhiteTurn{false}, SquareIndex::c8) && !isTargeted(board, WhiteTurn{false}, SquareIndex::d8)) {
            moves.push_back({.flag=CASTLE, .castleMove=CastleMove{e8, c8, WHITE_KING, a8, d8, WHITE_ROOK}});
        }
    }
}

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
static void addSinglePawnMoveWhite(std::vector<Move>& moves, const Board& board, SquareIndex startPos, SquareIndex endPos) {
    if ((1ULL << endPos) & 0x8080808080808080) {
        //promotion moves
        moves.push_back({.flag=PROMOTION, .promotionMove=PromotionMove{startPos, endPos, WHITE_PAWN, WHITE_QUEEN, board.getType(endPos)}});
        moves.push_back({.flag=PROMOTION, .promotionMove=PromotionMove{startPos, endPos, WHITE_PAWN, WHITE_BISHOP, board.getType(endPos)}});
        moves.push_back({.flag=PROMOTION, .promotionMove=PromotionMove{startPos, endPos, WHITE_PAWN, WHITE_KNIGHT, board.getType(endPos)}});
        moves.push_back({.flag=PROMOTION, .promotionMove=PromotionMove{startPos, endPos, WHITE_PAWN, WHITE_ROOK, board.getType(endPos)}});
    }
    else {
        //normal move
        moves.push_back({.flag=NORMAL, .normalMove=NormalMove{startPos, endPos, WHITE_PAWN, board.getType(endPos)}});
    }
}

static void addPawnPushMovesBlack(std::vector<Move>& moves, const Board& board, uint64_t pawns, uint64_t unoccupied) {
    uint64_t pushMoves = generatePawnPushBitboard(WhiteTurn{false}, pawns, unoccupied);

    while (pushMoves) {
        SquareIndex targetSquareIndex = (SquareIndex)__builtin_ctzll(pushMoves);
        uint64_t targetSquare = 1ULL << targetSquareIndex;

        if (northOne(targetSquare) & pushMoves) {
            //add double and single push move
            addSinglePawnMoveBlack(moves, board, northOne(targetSquareIndex), targetSquareIndex);
            addSinglePawnMoveBlack(moves, board, northOne(targetSquareIndex), targetSquareIndex);

            pushMoves &= pushMoves-1;
        }
        else {
            //add single push move
            addSinglePawnMoveBlack(moves, board, northOne(targetSquareIndex), targetSquareIndex);
        }

        pushMoves &= pushMoves-1;
    }
}
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
static void addSinglePawnMoveBlack(std::vector<Move>& moves, const Board& board, SquareIndex startPos, SquareIndex endPos) {
    if ((1ULL << endPos) & 0x0101010101010101) {
        //promotion moves
        moves.push_back({.flag=PROMOTION, .promotionMove=PromotionMove{startPos, endPos, BLACK_PAWN, BLACK_QUEEN, board.getType(endPos)}});
        moves.push_back({.flag=PROMOTION, .promotionMove=PromotionMove{startPos, endPos, BLACK_PAWN, BLACK_BISHOP, board.getType(endPos)}});
        moves.push_back({.flag=PROMOTION, .promotionMove=PromotionMove{startPos, endPos, BLACK_PAWN, BLACK_KNIGHT, board.getType(endPos)}});
        moves.push_back({.flag=PROMOTION, .promotionMove=PromotionMove{startPos, endPos, BLACK_PAWN, BLACK_ROOK, board.getType(endPos)}});
    }
    else {
        //normal move
        moves.push_back({.flag=NORMAL, .normalMove=NormalMove{startPos, endPos, BLACK_PAWN, board.getType(endPos)}});
    }
}
