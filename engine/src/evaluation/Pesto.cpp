/* Modified from Ronald Friederich's code from https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function */
#include "evaluation/Pesto.hpp"

#include "board/Board.hpp"
#include "board/BoardUtil.hpp"
#include "board/moveGeneration/MoveGenerator.hpp"

#define FLIP(sq) ((sq)^56)
#define OTHER(side) ((side)^ 1)

int leafNodesEvaluated = 0;

const int PAWN = 0;
const int KING = 5;

const int CHEKMATE_ABSOLUTE_SCORE = 9999999;

const int mg_value[6] = { 82, 337, 365, 477, 1025,  0};
const int eg_value[6] = { 94, 281, 297, 512,  936,  0};

/* values from Rofchade: http://www.talkchess.com/forum3/viewtopic.php?f=2&t=68311&start=19 */
/* values also reordered to match my board representation */
const int mg_pawn_table[64] = {
    0, -35, -26, -27, -14, -6, 98, 0,
    0, -1, -4, -2, 13, 7, 134, 0,
    0, -20, -4, -5, 6, 26, 61, 0,
    0, -23, -10, 12, 21, 31, 95, 0,
    0, -15, 3, 17, 23, 65, 68, 0,
    0, 24, 3, 6, 12, 56, 126, 0,
    0, 38, 33, 10, 17, 25, 34, 0,
    0, -22, -12, -25, -23, -20, -11, 0
};

const int eg_pawn_table[64] = {
    0, 13, 4, 13, 32, 94, 178, 0,
    0, 8, 7, 9, 24, 100, 173, 0,
    0, 8, -6, -3, 13, 85, 158, 0,
    0, 10, 1, -7, 5, 67, 134, 0,
    0, 13, 0, -7, -2, 56, 147, 0,
    0, 0, -5, -8, 4, 53, 132, 0,
    0, 2, -1, 3, 17, 82, 165, 0,
    0, -7, -8, -1, 17, 84, 187, 0
};

const int mg_knight_table[64] = {
    -105, -29, -23, -13, -9, -47, -73, -167,
    -21, -53, -9, 4, 17, 60, -41, -89,
    -58, -12, 12, 16, 19, 37, 72, -34,
    -33, -3, 10, 13, 53, 65, 36, -49,
    -17, -1, 19, 28, 37, 84, 23, 61,
    -28, 18, 17, 19, 69, 129, 62, -97,
    -19, -14, 25, 21, 18, 73, 7, -15,
    -23, -19, -16, -8, 22, 44, -17, -107
};

const int eg_knight_table[64] = {
    -29, -42, -23, -18, -17, -24, -25, -58,
    -51, -20, -3, -6, 3, -20, -8, -38,
    -23, -10, -1, 16, 22, 10, -25, -13,
    -15, -5, 15, 25, 22, 9, -2, -28,
    -22, -2, 10, 16, 22, -1, -9, -31,
    -18, -20, -3, 17, 11, -9, -25, -27,
    -50, -23, -20, 4, 8, -19, -24, -63,
    -64, -44, -22, -18, -18, -41, -52, -99
};

const int mg_bishop_table[64] = {
    -33, 4, 0, -6, -4, -16, -26, -29,
    -3, 15, 15, 13, 5, 37, 16, 4,
    -14, 16, 15, 13, 19, 43, -18, -82,
    -21, 0, 15, 26, 50, 40, -13, -37,
    -13, 7, 14, 34, 37, 35, 30, -25,
    -12, 21, 27, 12, 37, 50, 59, -42,
    -39, 33, 18, 10, 7, 37, 18, 7,
    -21, 1, 10, 4, -2, -2, -47, -8
};

const int eg_bishop_table[64] = {
    -23, -14, -12, -6, -3, 2, -8, -14,
    -9, -18, -3, 3, 9, -8, -4, -21,
    -23, -7, 8, 13, 12, 0, 7, -11,
    -5, -1, 10, 19, 9, -1, -12, -8,
    -9, 4, 13, 7, 14, -2, -3, -7,
    -16, -9, 3, 10, 10, 6, -13, -9,
    -5, -15, -7, -3, 3, 0, -4, -17,
    -17, -27, -15, -9, 2, 4, -14, -24
};

const int mg_rook_table[64] = {
    -19, -44, -45, -36, -24, -5, 27, 32,
    -13, -16, -25, -26, -11, 19, 32, 42,
    1, -20, -16, -12, 7, 26, 58, 32,
    17, -9, -17, -1, 26, 36, 62, 51,
    16, -1, 3, 9, 24, 17, 80, 63,
    7, 11, 0, -7, 35, 45, 67, 9,
    -37, -6, -5, 6, -8, 61, 26, 31,
    -26, -71, -33, -23, -20, 16, 44, 43
};

const int eg_rook_table[64] = {
    -9, -6, -4, 3, 4, 7, 11, 13,
    2, -6, 0, 5, 3, 7, 13, 10,
    3, 0, -5, 8, 13, 7, 13, 18,
    -1, 2, -1, 4, 1, 5, 11, 15,
    -5, -9, -7, -5, 2, 4, -3, 12,
    -13, -9, -12, -6, 1, -3, 3, 12,
    4, -11, -8, -8, -1, -5, 8, 8,
    -20, -3, -16, -11, 2, -3, 3, 5
};

const int mg_queen_table[64] = {
    -1, -35, -14, -9, -27, -13, -24, -28,
    -18, -8, 2, -26, -27, -17, -39, 0,
    -9, 11, -11, -9, -16, 7, -5, 29,
    10, 2, -2, -10, -16, 8, 1, 12,
    -15, 8, -5, -2, -1, 29, -16, 59,
    -25, 15, 2, -4, 17, 56, 57, 44,
    -31, -3, 14, 3, -2, 47, 28, 43,
    -50, 1, 5, -3, 1, 57, 54, 45
};

const int eg_queen_table[64] = {
    -33, -22, -16, -18, 3, -20, -17, -9,
    -28, -23, -27, 28, 22, 6, 20, 22,
    -22, -30, 15, 19, 24, 9, 32, 22,
    -43, -16, 6, 47, 45, 49, 41, 27,
    -5, -16, 9, 31, 57, 47, 58, 27,
    -32, -23, 17, 34, 40, 35, 25, 19,
    -20, -36, 10, 39, 57, 19, 30, 10,
    -41, -32, 5, 23, 36, 9, 0, 20
};

const int mg_king_table[64] = {
    -15, 1, -14, -49, -17, -9, 29, -65,
    36, 7, -14, -1, -20, 24, -1, 23,
    12, -8, -22, -27, -12, 2, -20, 16,
    -54, -64, -46, -39, -27, -16, -7, -15,
    8, -43, -44, -46, -30, -20, -8, -56,
    -28, -16, -30, -44, -25, 6, -4, -34,
    24, 9, -15, -33, -14, 22, -38, 2,
    14, 8, -27, -51, -36, -22, -29, 13
};

const int eg_king_table[64] = {
    -53, -27, -19, -18, -8, 10, -12, -74,
    -34, -11, -3, -4, 22, 17, 17, -35,
    -21, 4, 11, 21, 24, 23, 14, -18,
    -11, 13, 21, 24, 27, 15, 17, -18,
    -28, 14, 23, 27, 26, 20, 17, -11,
    -14, 4, 16, 23, 33, 45, 38, 15,
    -24, -5, 7, 9, 26, 44, 23, 4,
    -43, -17, -9, -11, 3, 13, 11, -17
};

const int* mg_pesto_table[6] =
{
    mg_pawn_table,
    mg_knight_table,
    mg_bishop_table,
    mg_rook_table,
    mg_queen_table,
    mg_king_table
};

const int* eg_pesto_table[6] =
{
    eg_pawn_table,
    eg_knight_table,
    eg_bishop_table,
    eg_rook_table,
    eg_queen_table,
    eg_king_table
};

const int gamephaseInc[12] = {0,0,1,1,1,1,2,2,4,4,0,0};
int mg_table[12][64];
int eg_table[12][64];

void initPestoTables()
{
    int pc, p, sq;
    for (p = PAWN, pc = PieceType::WHITE_PAWN; p <= KING; pc += 2, p++) {
        for (sq = 0; sq < 64; sq++) {
            mg_table[pc]  [sq] = mg_value[p] + mg_pesto_table[p][sq];
            eg_table[pc]  [sq] = eg_value[p] + eg_pesto_table[p][sq];
            mg_table[pc+1][sq] = mg_value[p] + mg_pesto_table[p][FLIP(sq)];
            eg_table[pc+1][sq] = eg_value[p] + eg_pesto_table[p][FLIP(sq)];
        }
    }
}

int pestoEval(const Board& boardRef)
{
    leafNodesEvaluated++;

    const std::array<int, 64>& board = boardRef.getMailboxBoard();
    int side2move = boardRef.getWhiteTurn();
    
    int mg[2];
    int eg[2];
    int gamePhase = 0;

    mg[PieceType::WHITE] = 0;
    mg[PieceType::BLACK] = 0;
    eg[PieceType::WHITE] = 0;
    eg[PieceType::BLACK] = 0;

    /* evaluate each piece */
    for (int sq = 0; sq < 64; ++sq) {
        int pc = board[sq];
        if (pc != PieceType::INVALID) {
            mg[PIECE_COLOUR(pc)] += mg_table[pc][sq];
            eg[PIECE_COLOUR(pc)] += eg_table[pc][sq];
            gamePhase += gamephaseInc[pc];
        }
    }

    /* tapered eval */
    int mgScore = mg[OTHER(side2move)] - mg[side2move];
    int egScore = eg[OTHER(side2move)] - eg[side2move];
    int mgPhase = gamePhase;
    if (mgPhase > 24) mgPhase = 24; /* in case of early promotion */
    int egPhase = 24 - mgPhase;
    return (mgScore * mgPhase + egScore * egPhase) / 24;
}

int terminalNodeEval(const Board& boardRef) {
    leafNodesEvaluated++;

    if (isKingTargeted(boardRef)) {
        //is checkmate
        return -CHEKMATE_ABSOLUTE_SCORE;
    }
    //stalemate
    return 0;
}
