#include "Engine.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "board/Board.hpp"
#include "board/BoardUtil.hpp"
#include "board/Move.hpp"
#include "board/moveGeneration/MoveGenerator.hpp"

// * ---------------------------------- [ CONSTRUCTORS/DESCTUCTOR ] ---------------------------------- * //

/**
 * Defualt constructor, takes no arguments and sets up default values for the engine
 */
Engine::Engine() {
    board->setDefaultBoard();
    // parseFen("8/P7/7P/8/8/p7/7p/8 w KQkq - 0 1");
    printASCIIBoard();

    Move moves[] = {
        // {.flag=NORMAL, .normalMove=NormalMove{b1, b3, WHITE_KNIGHT}},
        // {.flag=NORMAL, .normalMove=NormalMove{c1, c3, WHITE_BISHOP}},
        // {.flag=NORMAL, .normalMove=NormalMove{d1, d3, WHITE_QUEEN}},
        // {.flag=NORMAL, .normalMove=NormalMove{f1, f3, WHITE_BISHOP}},
        // {.flag=NORMAL, .normalMove=NormalMove{g1, g3, WHITE_KNIGHT}},

        // {.flag=NORMAL, .normalMove=NormalMove{b8, b6, BLACK_KNIGHT}},
        // {.flag=NORMAL, .normalMove=NormalMove{c8, c6, BLACK_BISHOP}},
        // {.flag=NORMAL, .normalMove=NormalMove{d8, d6, BLACK_QUEEN}},
        // {.flag=NORMAL, .normalMove=NormalMove{f8, f6, BLACK_BISHOP}},
        // {.flag=NORMAL, .normalMove=NormalMove{g8, g6, BLACK_KNIGHT}},
    };
    for (auto& i : moves) {
        board->makeMove(i);
        printASCIIBoard();
    }

    std::vector<Move> moves2 = generateMoves(*board, whiteTurn);
    for (auto& i : moves2) {
        board->makeMove(i);
        printASCIIBoard();
        board->unMakeMove(i);
        printASCIIBoard();
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
