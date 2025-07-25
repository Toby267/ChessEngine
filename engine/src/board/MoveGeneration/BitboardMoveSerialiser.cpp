#include "board/MoveGeneration/BitboardMoveSerialiser.hpp"

#include <cstdint>
#include <vector>

#include "board/BoardUtil.hpp"
#include "board/Move.hpp"
#include "board/Board.hpp"
#include "board/MoveGeneration/BitboardMoveGenerator.hpp"



/*
TODO: should split move generation files like so:

MoveGenerator.hpp

MoveGenerator.cpp
SimpleMoveGeneration.cpp
SlidingMoveGeneration.cpp
PawnMoveGeneration.cpp
SpecialMoveGeneration.cpp

where each have the code for bitboard move generation and bitboard move serialisation.
and each expose those two methods in the header file while helper methods are static
and the isTargeted() and the main moveGeneration functiions are in MoveGenerator.cpp
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ STATIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

static std::vector<Move> generateCastlingMovesWhite(const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData);
static std::vector<Move> generateCastlingMovesBlack(const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ PUBLIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//generates a vector of all king moves
std::vector<Move> generateKingMoves(WhiteTurn whiteTurn, uint64_t king, uint64_t friendlyPieces) {
    std::vector<Move> moves;
    moves.reserve(8);

    const SquareIndex   startSquare = (SquareIndex)__builtin_ctzll(king);
    const PieceType     pieceType = whiteTurn ? WHITE_KING : BLACK_KING;

    uint64_t movesBitboard = generateKingBitboard(king, friendlyPieces);
    while (movesBitboard) {
        SquareIndex targetSquare = (SquareIndex)__builtin_ctzll(movesBitboard);

        //TODO: this method creates a temporary object for move and copies it, need to find a way to use emplace_back
        moves.push_back({.flag=NORMAL, .normalMove=NormalMove{startSquare, targetSquare, pieceType}});

        movesBitboard &= movesBitboard-1;
    }

    return moves;
}

//generates a vector of all knight moves
std::vector<Move> generateKnightMoves(WhiteTurn whiteTurn, uint64_t knights, uint64_t friendlyPieces) {
    std::vector<Move> moves;
    moves.reserve(8);

    const PieceType pieceType = whiteTurn ? WHITE_KNIGHT : BLACK_KNIGHT;

    while (knights) {
        SquareIndex startSquare = (SquareIndex)__builtin_ctzll(knights);

        uint64_t movesBitboard = generateKnightBitboardSingular(startSquare, friendlyPieces);
        while (movesBitboard) {
            SquareIndex targetSquare = (SquareIndex)__builtin_ctzll(movesBitboard);

            moves.push_back({.flag=NORMAL, .normalMove=NormalMove{startSquare, targetSquare, pieceType}});

            movesBitboard &= movesBitboard-1;
        }

        knights &= knights-1;
    }

    return moves;
}

//generates a vector of all rook moves
std::vector<Move> generateRookMoves(WhiteTurn whiteTurn, uint64_t rooks, uint64_t occupied, uint64_t friendlyPieces) {
    std::vector<Move> moves;
    moves.reserve(8);

    const PieceType pieceType = whiteTurn ? WHITE_ROOK : BLACK_ROOK;

    while (rooks) {
        SquareIndex startSquare = (SquareIndex)__builtin_ctzll(rooks);

        uint64_t movesBitboard = generateRookBitboardSingular(startSquare, occupied, friendlyPieces);
        while (movesBitboard) {
            SquareIndex targetSquare = (SquareIndex)__builtin_ctzll(movesBitboard);

            moves.push_back({.flag=NORMAL, .normalMove=NormalMove{startSquare, targetSquare, pieceType}});

            movesBitboard &= movesBitboard-1;
        }

        rooks &= rooks-1;
    }

    return moves;
}

//generates a vector of all bishop moves
std::vector<Move> generateBishopMoves(WhiteTurn whiteTurn, uint64_t bishops, uint64_t occupied, uint64_t friendlyPieces) {
    std::vector<Move> moves;
    moves.reserve(8);

    const PieceType pieceType = whiteTurn ? WHITE_BISHOP : BLACK_BISHOP;

    while (bishops) {
        SquareIndex startSquare = (SquareIndex)__builtin_ctzll(bishops);

        uint64_t movesBitboard = generateBishopBitboardSingular(startSquare, occupied, friendlyPieces);
        while (movesBitboard) {
            SquareIndex targetSquare = (SquareIndex)__builtin_ctzll(movesBitboard);

            moves.push_back({.flag=NORMAL, .normalMove=NormalMove{startSquare, targetSquare, pieceType}});

            movesBitboard &= movesBitboard-1;
        }

        bishops &= bishops-1;
    }

    return moves;
}

//generates a vector of all queen moves
std::vector<Move> generateQueenMoves(WhiteTurn whiteTurn, uint64_t queens, uint64_t occupied, uint64_t friendlyPieces) {
    std::vector<Move> moves;
    moves.reserve(8);

    const PieceType pieceType = whiteTurn ? WHITE_QUEEN : BLACK_QUEEN;

    while (queens) {
        SquareIndex startSquare = (SquareIndex)__builtin_ctzll(queens);

        uint64_t movesBitboard = generateQueenBitboardSingular(startSquare, occupied, friendlyPieces);
        while (movesBitboard) {
            SquareIndex targetSquare = (SquareIndex)__builtin_ctzll(movesBitboard);

            moves.push_back({.flag=NORMAL, .normalMove=NormalMove{startSquare, targetSquare, pieceType}});

            movesBitboard &= movesBitboard-1;
        }

        queens &= queens-1;
    }

    return moves;
}

//generates a vector of all pawn moves
std::vector<Move> generatePawnMoves(WhiteTurn whiteTurn, uint64_t pawns, uint64_t unoccupied, uint64_t oppositionPieces) {
    std::vector<Move> moves;
    moves.reserve(8);

    const PieceType pieceType = whiteTurn ? WHITE_PAWN : BLACK_PAWN;

    uint64_t pushMoves = generatePawnPushBitboard(whiteTurn, pawns, unoccupied);
    while (pushMoves) {
        SquareIndex targetSquare = (SquareIndex)__builtin_ctzll(pushMoves);

        //add single push move
        moves.push_back({.flag=NORMAL, .normalMove=NormalMove{}});

        pushMoves &= pushMoves-1;

        if (targetSquare+1 & pushMoves) {//not fine
            //add double push move

            pushMoves &= pushMoves-1;
        }
    }

    uint64_t attackMoves = generatePawnAttackBitboard(whiteTurn, pawns, oppositionPieces);


    
    return moves;
}

//generates a vector of all en passant moves
std::vector<Move> generateEnPassantMoves(WhiteTurn whiteTurn, uint64_t friendlyPieces, std::array<__uint128_t, 16> enPassantData) {
    std::vector<Move> moves;
    moves.reserve(1);

    //very cursed line
    SquareIndex (*getEndPos)(int) = whiteTurn ? (SquareIndex(*)(int))northOne : (SquareIndex(*)(int))southOne;

    for (int i = 0; i < enPassantData.size(); i++) {
        if (!(enPassantData[i] & 1)) continue;

        int killIndex = ((i % 8) * 8) + ((i > 7) ? 4 : 3);
        uint64_t pawnBitboard = 1ULL << killIndex;

        if (pawnBitboard & friendlyPieces) continue;

        if (westOne(pawnBitboard) & friendlyPieces) {
            moves.push_back({.flag=EN_PASSANT, .enPassantMove=EnPassantMove{westOne(killIndex), getEndPos(killIndex), WHITE_PAWN, (SquareIndex)killIndex, BLACK_PAWN}});
        }
        else if (eastOne(pawnBitboard) & friendlyPieces) {
            moves.push_back({.flag=EN_PASSANT, .enPassantMove=EnPassantMove{eastOne(killIndex), getEndPos(killIndex), WHITE_PAWN, (SquareIndex)killIndex, BLACK_PAWN}});
        }
    }

    return moves;
}

//generates a vector of all castling moves
std::vector<Move> generateCastlingMoves(WhiteTurn whiteTurn, const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData) {
    if (whiteTurn)
        return generateCastlingMovesWhite(board, occupied, castleData);
    else
        return generateCastlingMovesBlack(board, occupied, castleData);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ STATIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//generates a vector of all castling moves for the white pieces
static std::vector<Move> generateCastlingMovesWhite(const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData) {
    std::vector<Move> moves;
    moves.reserve(2);

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

    return moves;
}
//generates a vector of all castling moves for the black pieces
static std::vector<Move> generateCastlingMovesBlack(const Board &board, uint64_t occupied, std::array<__uint128_t, 4> castleData) {
    std::vector<Move> moves;
    moves.reserve(2);

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

    return moves;
}
