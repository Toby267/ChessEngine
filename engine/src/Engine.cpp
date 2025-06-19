#include "Engine.hpp"

#include <iostream>
#include <string>

#include "board/Board.hpp"

// * ---------------------------------- [ CONSTRUCTORS/DESCTUCTOR ] ---------------------------------- * //

Engine::Engine() {
    board = new Board();
    board->setupDefaultBoard();
    //parseFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    //parseFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    //parseFen("rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2");
    //parseFen("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
    printASCIIBoard();
}

Engine::~Engine() {
    delete board;
}

// * ---------------------------------- [ PUBLIC METHODS ] ----------------------------------- * //

void Engine::parseFen(const std::string& FEN) {
    //TODO: this
    /*
        definitely needs 1, 3, and 4
        will probabably store 5 but this may be stored in Engine.cpp
        2 will be stored in Engine.cpp
        and 6 is useless
    */

    std::cout << FEN << '\n';

    const std::string fenChars = "KQBNRPkqbnrp";
    int i = 0;

    //parses the first part of the FEN - piece positions
    for (int file = -1, rank = 7; i < FEN.length(); i++) {
        if (FEN[i] == ' ') break;
        
        size_t index = fenChars.find(FEN[i]);
        file++;

        if (FEN[i] == '/') {
            rank--;
            file = -1;
        }
        else if (index == -1) {
            file += FEN[i] - '0' - 1;
        }
        else {
            board->addPiece((PieceType)index, (SquareIndex)(8*file+rank));
        }
    }

    //parses the second part of the FEN
    for (i++; i < FEN.length(); i++) {
        if (FEN[i] == ' ') break;
        std::cout << ':' << FEN[i] << '\n';

        whiteTurn = FEN[i] == 'w' ? 1 : 0;
    }


    board->setRookCastleable(SquareIndex::h1, false);
    board->setRookCastleable(SquareIndex::a1, false);
    board->setRookCastleable(SquareIndex::h8, false);
    board->setRookCastleable(SquareIndex::a1, false);

    //parses the third part of the FEN
    for (i++; i < FEN.length(); i++) {
        if (FEN[i] == ' ') break;
        std::cout << ':' << FEN[i] << '\n';

        switch (FEN[i]) {
            case 'K':
                board->setRookCastleable(SquareIndex::h1, true);
                break;
            case 'Q':
                board->setRookCastleable(SquareIndex::a1, true);
                break;
            case 'k':
                board->setRookCastleable(SquareIndex::h8, true);
                break;
            case 'q':
                board->setRookCastleable(SquareIndex::a8, true);
                break;
        }
    }

    //parses the fourth part of the FEN
    i++;

    if (FEN[i] == '-') {
        board->setEnPassantSquareOff();
    }
    else {
        int thing = 8 * (FEN[i] - 'a');
        thing += FEN[i+1] - '1';

        board->setEnPassantSquare((SquareIndex)thing);
    }

    for (; i < FEN.length(); i++) {
        if (FEN[i] == ' ') break;
    }

    //parses the fifth part of the FEN
    for (i++; i < FEN.length(); i++) {
        if (FEN[i] == ' ') break;
        std::cout << ':' << FEN[i] << '\n';

        drawMoveCounter = FEN[i] - '0';
    }
}

// * ---------------------------------- [ PRIVATE METHODS ] ----------------------------------- * //

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
