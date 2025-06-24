#include <board/BoardEnums.hpp>

uint64_t northOne(uint64_t b) {
    return (b << 1) & 0xFEFEFEFEFEFEFEFE;
}
uint64_t southOne(uint64_t b) {
    return (b >> 1) & 0x7F7F7F7F7F7F7F7F;
}
uint64_t eastOne(uint64_t b) {
    return (b << 8) & 0x00FFFFFFFFFFFFFF;
}
uint64_t westOne(uint64_t b) {
    return (b >> 8) & 0xFFFFFFFFFFFFFF00;
}

uint64_t northEast(uint64_t b) {
    return (b << 9) & 0x00FEFEFEFEFEFEFE;
}
uint64_t northWest(uint64_t b) {
    return (b >> 7) & 0xFEFEFEFEFEFEFE00;
}
uint64_t southEast(uint64_t b) {
    return (b << 7) & 0x007F7F7F7F7F7F7F;
}
uint64_t southWest(uint64_t b) {
    return (b >> 9) & 0x7F7F7F7F7F7F7F00;
}
