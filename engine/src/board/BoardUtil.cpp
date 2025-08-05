#include "board/BoardUtil.hpp"

// * ------------------------------------ [ ORTHOGANOL SHIFTERS INT ] ------------------------------------ * //

//doesn't check for if the piece goes off the board
SquareIndex northOne(int piece) {
    return (SquareIndex)(piece+1);
}
SquareIndex southOne(int piece) {
    return (SquareIndex)(piece-1);
}
SquareIndex eastOne(int piece) {
    return (SquareIndex)(piece+8);
}
SquareIndex westOne(int piece) {
    return (SquareIndex)(piece-8);
}

SquareIndex northEastOne(int piece) {
    return (SquareIndex)(piece+9);
}
SquareIndex northWestOne(int piece) {
    return (SquareIndex)(piece-7);
}
SquareIndex southEastOne(int piece) {
    return (SquareIndex)(piece+7);
}
SquareIndex southWestOne(int piece) {
    return (SquareIndex)(piece-9);
}

// * ----------------------------------- [ ORTHOGANOL SHIFTERS UINT64 ] ---------------------------------- * //

//shifts the board square by one in each orthoganol direction
uint64_t northOne(uint64_t board) {
    return (board << 1) & 0xFEFEFEFEFEFEFEFEULL;
}
uint64_t southOne(uint64_t board) {
    return (board >> 1) & 0x7F7F7F7F7F7F7F7FULL;
}
uint64_t eastOne(uint64_t board) {
    return (board << 8) & 0xFFFFFFFFFFFFFF00ULL;
}
uint64_t westOne(uint64_t board) {
    return (board >> 8) & 0x00FFFFFFFFFFFFFFULL;
}

uint64_t northEastOne(uint64_t board) {
    return (board << 9) & 0xFEFEFEFEFEFEFE00ULL;
}
uint64_t northWestOne(uint64_t board) {
    return (board >> 7) & 0x00FEFEFEFEFEFEFEULL;
}
uint64_t southEastOne(uint64_t board) {
    return (board << 7) & 0x7F7F7F7F7F7F7F00ULL;
}
uint64_t southWestOne(uint64_t board) {
    return (board >> 9) & 0x007F7F7F7F7F7F7FULL;
}

// * ------------------------------------ [ DIRECTION RAY FUNCTIONS ] ------------------------------------ * //

//returns all square in a specific orthoganol direction given a square
uint64_t calcNorthMask(SquareIndex square) {
    return 2 * ((1ULL << (square|7)) - (1ULL << square));
}
uint64_t calcEastMask(SquareIndex square) {
    return 0x0101010101010100ULL << square;
}
uint64_t calcSouthMask(SquareIndex square) {
    return (1ULL << square) - (1ULL << (square & 56));
}
uint64_t calcWestMask(SquareIndex square) {
    return 0x0080808080808080ULL >> (square ^ 63);
}

uint64_t calcNorthEastMask(SquareIndex square){ 
    uint64_t diagonal = 0x8040201008040200ULL << (uint64_t)square;

    int n = (square & 7) + 1; //x%8 === x&7
    uint64_t mask = 0x0101010101010101ULL * ((1ULL << n) -1);

    return diagonal & ~mask;
}
uint64_t calcNorthWestMask(SquareIndex square){
    uint64_t diagonal = 0x0102040810204000ULL >> (square ^ 63);

    int n = (square & 7) + 1; //x%8 === x&7
    uint64_t mask = 0x0101010101010101ULL * ((1ULL << n) -1);

    return diagonal & ~mask;
}
uint64_t calcSouthEastMask(SquareIndex square){ 
    uint64_t diagonal = 0x0002040810204080ULL << square;

    int n = (square & 7) + 1; //x%8 === x&7
    uint64_t mask = 0x0101010101010101ULL * ((1ULL << n) -1);

    return diagonal & mask;
}
uint64_t calcSouthWestMask(SquareIndex square){ 
    uint64_t diagonal = 0x0040201008040201ULL >> (square ^ 63);

    int n = (square & 7) + 1; //x%8 === x&7
    uint64_t mask = 0x0101010101010101ULL * ((1ULL << n) -1);

    return diagonal & mask;
}
