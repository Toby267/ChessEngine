#include "Engine.hpp"

#include <iostream>
#include <string>

#include "board/Board.hpp"
#include "board/BoardUtil.hpp"
#include "board/Move.hpp"
#include "board/MoveGeneration/MoveGenerator.hpp"
#include "board/MoveGeneration/BitboardMoveGenerator.hpp"

// * ---------------------------------- [ CONSTRUCTORS/DESCTUCTOR ] ---------------------------------- * //

/**
 * Defualt constructor, takes no arguments and sets up default values for the engine
 */
Engine::Engine() {
    board->setDefaultBoard();
    // parseFen("8/1r6/4Q3/8/8/8/1Q6/8 w KQkq - 0 1");
    printASCIIBoard();

    //TODO: fill this with moves to test the functionality of make and unmake
    Move moves[] = {
        {.flag=NORMAL, .normalMove=NormalMove{b1, b3, WHITE_KNIGHT}},
        {.flag=NORMAL, .normalMove=NormalMove{c1, c3, WHITE_BISHOP}},
        {.flag=NORMAL, .normalMove=NormalMove{d1, d3, WHITE_QUEEN}},
        {.flag=NORMAL, .normalMove=NormalMove{d2, d4, WHITE_PAWN}},
        // {.flag=NORMAL, .normalMove=NormalMove{c8, f4, BLACK_BISHOP}},
        // {.flag=NORMAL, .normalMove=NormalMove{b8, d2, BLACK_KNIGHT}},

        // {.flag=NORMAL, .normalMove=NormalMove{a2, a4, WHITE_PAWN}},
        // {.flag=NORMAL, .normalMove=NormalMove{b2, b4, WHITE_PAWN}},
        // {.flag=NORMAL, .normalMove=NormalMove{c2, c4, WHITE_PAWN}},
        // {.flag=NORMAL, .normalMove=NormalMove{d2, d4, WHITE_PAWN}},
        // {.flag=NORMAL, .normalMove=NormalMove{e2, e4, WHITE_PAWN}},
        // {.flag=NORMAL, .normalMove=NormalMove{f2, f4, WHITE_PAWN}},
        // {.flag=NORMAL, .normalMove=NormalMove{g2, g4, WHITE_PAWN}},
        // {.flag=NORMAL, .normalMove=NormalMove{h2, h4, WHITE_PAWN}},

        // {.flag=NORMAL, .normalMove=NormalMove{a7, a5, BLACK_PAWN}},
        // {.flag=NORMAL, .normalMove=NormalMove{b7, b5, BLACK_PAWN}},
        // {.flag=NORMAL, .normalMove=NormalMove{c7, c5, BLACK_PAWN}},
        // {.flag=NORMAL, .normalMove=NormalMove{d7, d5, BLACK_PAWN}},
        // {.flag=NORMAL, .normalMove=NormalMove{e7, e5, BLACK_PAWN}},
        // {.flag=NORMAL, .normalMove=NormalMove{f7, f5, BLACK_PAWN}},
        // {.flag=NORMAL, .normalMove=NormalMove{g7, g5, BLACK_PAWN}},
        // {.flag=NORMAL, .normalMove=NormalMove{h7, h5, BLACK_PAWN}},
    };

    for (auto& i : moves) {
        board->makeMove(i);
        printASCIIBoard();
    }

    generateMoves(*board, whiteTurn);
    printASCIIBoard();

    // bool a = isTargeted(*board, whiteTurn, SquareIndex::g5);
    // std::cout << "a is: " << a << '\n';
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
