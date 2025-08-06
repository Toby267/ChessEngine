#include "Engine.hpp"

int main(int argc, char *argv[]) {
    switch (argc) {
        case 1:     { Engine engine{};                              break; }
        case 2:     { Engine engine(UserColour(argv[1][0] != 'w')); break; }
        default:    { break; }
    }
    
    return 0;
}
