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

    //TODO: fill this with moves to test the functionality of make and unmake
    Move moves[] = {
        {.flag=MoveType::NORMAL, .normalMove=NormalMove{SquareIndex::b1, SquareIndex::a3, PieceType::WHITE_KNIGHT, PieceType::INVALID}},
        {.flag=MoveType::NORMAL, .normalMove=NormalMove{SquareIndex::d2, SquareIndex::d4, PieceType::WHITE_PAWN, PieceType::INVALID}},
        {.flag=MoveType::NORMAL, .normalMove=NormalMove{SquareIndex::c1, SquareIndex::e3, PieceType::WHITE_BISHOP, PieceType::INVALID}},
        {.flag=MoveType::NORMAL, .normalMove=NormalMove{SquareIndex::d1, SquareIndex::d2, PieceType::WHITE_QUEEN, PieceType::INVALID}},
        
        {.flag=MoveType::NORMAL, .normalMove=NormalMove{SquareIndex::g1, SquareIndex::h3, PieceType::WHITE_KNIGHT, PieceType::INVALID}},
        {.flag=MoveType::NORMAL, .normalMove=NormalMove{SquareIndex::g2, SquareIndex::g3, PieceType::WHITE_PAWN, PieceType::INVALID}},
        {.flag=MoveType::NORMAL, .normalMove=NormalMove{SquareIndex::f1, SquareIndex::g2, PieceType::WHITE_BISHOP, PieceType::INVALID}},

        
        {.flag=MoveType::NORMAL, .normalMove=NormalMove{SquareIndex::b8, SquareIndex::a6, PieceType::BLACK_KNIGHT, PieceType::INVALID}},
        {.flag=MoveType::NORMAL, .normalMove=NormalMove{SquareIndex::d7, SquareIndex::d5, PieceType::BLACK_PAWN, PieceType::INVALID}},
        {.flag=MoveType::NORMAL, .normalMove=NormalMove{SquareIndex::c8, SquareIndex::e6, PieceType::BLACK_BISHOP, PieceType::INVALID}},
        {.flag=MoveType::NORMAL, .normalMove=NormalMove{SquareIndex::d8, SquareIndex::d7, PieceType::BLACK_QUEEN, PieceType::INVALID}},

        {.flag=MoveType::NORMAL, .normalMove=NormalMove{SquareIndex::g8, SquareIndex::h6, PieceType::BLACK_KNIGHT, PieceType::INVALID}},
        {.flag=MoveType::NORMAL, .normalMove=NormalMove{SquareIndex::g7, SquareIndex::g6, PieceType::BLACK_PAWN, PieceType::INVALID}},
        {.flag=MoveType::NORMAL, .normalMove=NormalMove{SquareIndex::f8, SquareIndex::g7, PieceType::BLACK_BISHOP, PieceType::INVALID}},

        {.flag=MoveType::NORMAL, .normalMove=NormalMove{SquareIndex::a1, SquareIndex::b1, PieceType::WHITE_ROOK, PieceType::INVALID}},
        {.flag=MoveType::NORMAL, .normalMove=NormalMove{SquareIndex::b1, SquareIndex::a1, PieceType::WHITE_ROOK, PieceType::INVALID}},
    };

    for (auto& i : moves) {
        board->makeMove(i);
        printASCIIBoard();
        board->printDebugData();
    }

    generateMoves(*board, whiteTurn, SquareIndex::d2);
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
