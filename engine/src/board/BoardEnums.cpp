#include <board/BoardEnums.hpp>

uint64_t eastOne(uint64_t b) {
    return (b << 8) & ~0xFF00000000000000;
}
uint64_t westOne(uint64_t b) {
    return (b >> 8) & ~0x00000000000000FF;
}
uint64_t northOne(uint64_t b) {
    return (b << 1) & ~0x0101010101010101;
}
uint64_t southOne(uint64_t b) {
    return (b >> 1) & ~0x8080808080808080;
}
