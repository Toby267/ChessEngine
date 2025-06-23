#include "Engine.hpp"

#include <iostream>
#include <string>

#include "board/Board.hpp"
#include "board/MoveGenerator.hpp"

// * ---------------------------------- [ CONSTRUCTORS/DESCTUCTOR ] ---------------------------------- * //

/**
 * Defualt constructor, takes no arguments and sets up default values for the engine
 */
Engine::Engine() {
    board->parseFen("8/8/8/4K3/8/8/8/8 w KQkq - 0 1");
    generateMoves(*board, whiteTurn);
    printASCIIBoard();
}

Engine::~Engine() {
    delete board;
}

// * ---------------------------------- [ PUBLIC METHODS ] ----------------------------------- * //

/**
 * Parses a given fen, storing relevent information here, and passing it to the board to parse there
 * 
 * @param FEN the fen that is to be parsed
*/
void Engine::parseFen(const std::string& FEN) {
    std::cout << FEN << '\n';
    
    board->parseFen(FEN);

    int i;

    //skips the first part of the FEN
    for (i = 0; i < FEN.length() && FEN[i] != ' '; i++)
        ;

    //parses the second part of the FEN
    for (i++; i < FEN.length(); i++) {
        if (FEN[i] == ' ') break;

        whiteTurn = FEN[i] == 'w' ? 1 : 0;
    }

    //skips the third & fourth part of the FEN
    for (i++; i < FEN.length() && FEN[i] != ' '; i++)
        ;
    for (i++; i < FEN.length() && FEN[i] != ' '; i++)
        ;

    //parses the fifth part of the FEN
    i++;
    if (FEN[i+1] == ' ')
        drawMoveCounter = FEN[i] - '0';
    else
        drawMoveCounter = 10* (FEN[i] - '0') + (FEN[i+1] - '0');
}

// * ---------------------------------- [ PRIVATE METHODS ] ----------------------------------- * //

/**
 * Prints out the board using unicode characters for the chess pieces
 */
void Engine::printASCIIBoard() {
    const std::string pieceChars[] = {"o", "♚", "♛", "♝", "♞", "♜", "♟", "♔", "♕", "♗", "♘", "♖", "♙"};
    
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            PieceType type = board->getType((SquareIndex)(8*file+rank));

            std::cout << pieceChars[(int)type+1] << ' ';
        }
        std::cout << '\n';
    }

    std::cout << '\n';
}
