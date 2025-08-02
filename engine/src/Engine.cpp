#include "Engine.hpp"

#include <cstdint>
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
    for (int i = 1; i <= 5; i++) {
        // board->setDefaultBoard();                                                                //position 1                    - bug at depth 5, off by ~300
        // parseFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");        //position 2                    - bug at depth 3, off by 30
        // parseFen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");                                   //position 3                    - bug at depth 4, off by 4
        // parseFen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");            //position 4                    - bug at depth 4, off by 20          
            // parseFen("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ");       //alternate to position 4       - bug at depth 4, off by 20
        // parseFen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");                 //position 5                    - bug at depth 3, off by 3
        parseFen("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");   //position 6                    - bug at depth 2, off by 1      (should be 2079, but is 2080)

        std::cout << "depth: " << i << " nodes: " << perft(i) << '\n';
    }

    // parseFen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    // perftDivide(2);
}

Engine::~Engine() {
    delete board;
}

// * ---------------------------------- [ PUBLIC METHODS ] ----------------------------------- * //

uint64_t Engine::perft(int depth) {
    std::vector<Move> moves;
    uint64_t nodes = 0;

    if (depth == 0)
        return  1ULL;

    moves = generateMoves(*board, whiteTurn);

    for (auto& i : moves) {
        board->makeMove(i);
        whiteTurn = !whiteTurn;

        nodes += perft(depth -1);

        board->unMakeMove(i);
        whiteTurn = !whiteTurn;
    }

    return nodes;
}

uint64_t Engine::perftDivide(int depth) {
    std::vector<Move> moves;
    std::vector<int> childMoveCount;
    uint64_t nodes = 0;

    if (depth == 0)
        return  1ULL;

    moves = generateMoves(*board, whiteTurn);

    for (auto& i : moves) {
        board->makeMove(i);
        whiteTurn = !whiteTurn;

        uint64_t childMoves = perftDivide(depth -1);
        nodes += childMoves;
        childMoveCount.push_back(childMoves);

        board->unMakeMove(i);
        whiteTurn = !whiteTurn;
    }

    std::cout << "=============================== " << "depth: " << depth << " ===============================" << '\n';
    for (int i = 0; i < moves.size(); i++) {
        printMove(moves[i]);
        std::cout << "child moves: " << childMoveCount[i] << '\n' << '\n';
    }

    return nodes;
}

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
