#include "Engine.hpp"

#include <iostream>
#include <string>

#include "board/Board.hpp"
#include "board/BoardUtil.hpp"
#include "board/Move.hpp"
#include "board/MoveGenerator.hpp"

// * ---------------------------------- [ CONSTRUCTORS/DESCTUCTOR ] ---------------------------------- * //

/**
 * Defualt constructor, takes no arguments and sets up default values for the engine
 */
Engine::Engine() {
    board->setDefaultBoard();
    printASCIIBoard();
    board->printDebugData();

    //TODO: fill this with moves to test the functionality of make and unmake
    Move moves[] = {
        //{.flag=MoveType::NORMAL, .normalMove=NormalMove{SquareIndex::e1, SquareIndex::e3, PieceType::WHITE_KING, PieceType::INVALID}},
        {.flag=PROMOTION, .promotionMove={a2, a8, WHITE_PAWN, WHITE_QUEEN, BLACK_ROOK}},
        {.flag=NORMAL, .normalMove={a8, d4, WHITE_QUEEN, INVALID}}
    };

    for (auto& i : moves) {
        board->makeMove(i);
        printASCIIBoard();
        board->printDebugData();
    }
    for (int i = sizeof(moves)/sizeof(moves[0]) -1; i >= 0; i--) {
        board->unMakeMove(moves[i]);
        printASCIIBoard();
        board->printDebugData();
    }
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
    board->parseFen(FEN);

    int i;

    //skips the first part of the FEN
    for (i = 0; i < FEN.length() && FEN[i] != ' '; i++)
        ;

    //parses the second part of the FEN
    for (i++; i < FEN.length(); i++) {
        if (FEN[i] == ' ') break;

        whiteTurn = FEN[i] == 'w' ? true : false;
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
