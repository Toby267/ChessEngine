#include "Engine.hpp"

#include <iostream>
#include <string>

#include "board/Board.hpp"

// * ---------------------------------- [ CONSTRUCTORS/DESCTUCTOR ] ---------------------------------- * //

Engine::Engine() {
    board = new Board();
    printASCIIBoard();

    Move myMove1 = {.flag=MoveType::NORMAL,     .normalMove=NormalMove          {SquareIndex::a1, SquareIndex::c7, PieceType::WHITE_ROOK, PieceType::BLACK_PAWN}};
    Move myMove2 = {.flag=MoveType::PROMOTION,  .promotionMove=PromotionMove    {SquareIndex::a1, SquareIndex::c7, PieceType::WHITE_ROOK, PieceType::WHITE_QUEEN, PieceType::INVALID}};
    Move myMove3 = {.flag=MoveType::EN_PASSANT, .enPassantMove=EnPassantMove    {SquareIndex::a1, SquareIndex::c7, PieceType::WHITE_ROOK, SquareIndex::a7, PieceType::BLACK_PAWN}};
    Move myMove4 = {.flag=MoveType::CASTLE,     .castleMove=CastleMove          {SquareIndex::a1, SquareIndex::c5, PieceType::WHITE_ROOK, SquareIndex::b1, SquareIndex::d5, PieceType::WHITE_KNIGHT}};
    
    board->makeMove(myMove4);
    printASCIIBoard();
    
    board->makeMove(myMove4);
    printASCIIBoard();
}

Engine::~Engine() {
    delete board;
}

// * ---------------------------------- [ PRIVATE METHODS ] ----------------------------------- * //

void Engine::printASCIIBoard() {
    const std::string pieceChars[] = {"♚", "♛", "♝", "♞", "♜", "♟", "♔", "♕", "♗", "♘", "♖", "♙"};
    
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            PieceType type = board->getType((SquareIndex)(8*file+rank));

            if (type == 14)
                std::cout << "o" << ' ';
            else
                std::cout << pieceChars[(int)type] << ' ';
        }
        std::cout << '\n';
    }

    std::cout << '\n';
}
