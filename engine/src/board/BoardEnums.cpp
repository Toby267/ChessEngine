#include <board/BoardEnums.hpp>

#include <bitset>
#include <iostream>

//shifts a square by one in each orthoganol direction
uint64_t northOne(uint64_t b) {
    return (b << 1) & 0xFEFEFEFEFEFEFEFE;
}
uint64_t southOne(uint64_t b) {
    return (b >> 1) & 0x7F7F7F7F7F7F7F7F;
}
uint64_t eastOne(uint64_t b) {
    return (b << 8) & 0xFFFFFFFFFFFFFF00;
}
uint64_t westOne(uint64_t b) {
    return (b >> 8) & 0x00FFFFFFFFFFFFFF;
}

uint64_t northEastOne(uint64_t b) {
    return (b << 9) & 0xFEFEFEFEFEFEFE00;
}
uint64_t northWestOne(uint64_t b) {
    return (b >> 7) & 0x00FEFEFEFEFEFEFE;
}
uint64_t southEastOne(uint64_t b) {
    return (b << 7) & 0x7F7F7F7F7F7F7F00;
}
uint64_t southWestOne(uint64_t b) {
    return (b >> 9) & 0x007F7F7F7F7F7F7F;
}

//returns all square in a specific orthoganol direction given a square
uint64_t calcNorthMask(SquareIndex square) {
    return 2 * ((1ULL << (square|7)) - (1ULL << square));
}
uint64_t calcEastMask(SquareIndex square) {
    return (uint64_t)(0x0101010101010100) << square;
}
uint64_t calcSouthMask(SquareIndex square) {
    return (1ULL << square) - (1ULL << (square & 56));
}
uint64_t calcWestMask(SquareIndex square) {
    return (uint64_t)(0x0080808080808080) >> (square ^ 63);
}

uint64_t calcNorthEastMask(SquareIndex square){ 
    uint64_t diagonal = (uint64_t)(0x8040201008040200) << (uint64_t)square;

    int n = (square & 7) + 1; //x%8 === x&7
    uint64_t mask = (uint64_t)0x0101010101010101 * ((1ULL << n) -1);

    return diagonal & ~mask;

    // uint64_t mask = 0;
    // for (int i = 0; i < (square & 7) + 1; i++)
    //     mask += 0x0101010101010101 << i;
    
    //the used method for the mask is equivalent to the loop as (1ULL << n) -1 is a mask equivalent to a*(2^n -1)
    //which is what the loop is doing: a*(2^0) + a*(2^1) + a*(2^2) + ... + a*(2^(n-1))
    //                                  = a*(2^0 + 2^1 + 2^2 + ... + 2^(n-1))
    //                                  = a*(2^n -1)
    //where a = 0x0101010101010101ULL
}
uint64_t calcNorthWestMask(SquareIndex square){
    uint64_t diagonal = (uint64_t)(0x0102040810204000) >> (square ^ 63);

    int n = (square & 7) + 1; //x%8 === x&7
    uint64_t mask = (uint64_t)0x0101010101010101 * ((1ULL << n) -1);

    return diagonal & ~mask;
}
uint64_t calcSouthEastMask(SquareIndex square){ 
    uint64_t diagonal = (uint64_t)(0x0002040810204080) << square;

    int n = (square & 7) + 1; //x%8 === x&7
    uint64_t mask = (uint64_t)0x0101010101010101 * ((1ULL << n) -1);

    return diagonal & mask;
}
uint64_t calcSouthWestMask(SquareIndex square){ 
    uint64_t diagonal = (uint64_t)(0x0040201008040201) >> (square ^ 63);

    int n = (square & 7) + 1; //x%8 === x&7
    uint64_t mask = (uint64_t)0x0101010101010101 * ((1ULL << n) -1);

    return diagonal & mask;
}
