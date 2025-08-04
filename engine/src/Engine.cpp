#include "Engine.hpp"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "Bot.hpp"
#include "board/Board.hpp"
#include "board/BoardUtil.hpp"
#include "board/Move.hpp"
#include "board/moveGeneration/MoveGenerator.hpp"

// * ---------------------------------- [ CONSTRUCTORS/DESCTUCTOR ] ---------------------------------- * //

/**
 * Defualt constructor, takes no arguments and sets up default values for the engine
 */
Engine::Engine() {    
    playMatch();
}
Engine::Engine(UserColour isBotWhite) : isBotWhite(!isBotWhite){
    playMatch();
}

Engine::~Engine() {
    delete board;
    delete bot;
}

// * ---------------------------------- [ PUBLIC METHODS ] ----------------------------------- * //

void Engine::playMatch() {
    //array of size 2 of the 'Playable' interface that contains the bot, and the player
    //use the whiteTurn variable to index the above array. it is a bool, which are implicitly converted to intergers under the hood

    for (;;) {
        printASCIIBoard();

        Move move = (board->getWhiteTurn() == isBotWhite) ? bot->getBestMove() : getUserMove();
        board->makeMove(move);

        if (getCurrentGameState() != GameState::Live) break;
    }

    GameState gameState = getCurrentGameState();
    
    if (gameState == GameState::Checkmate)
        std::cout << (board->getWhiteTurn() ? "Black Wins!" : "White Wins!") << '\n';
    else if (gameState == GameState::Stalemate)
        std::cout << "Draw";

    printASCIIBoard();
}

/**
 * Parses a given fen, storing relevent information here, and passing it to the board to parse there
 * 
 * @param FEN the fen that is to be parsed
*/
void Engine::parseFen(const std::string& FEN) {
    board->parseFen(FEN);

    int i;

    //skips the first, second, third & fourth part of the FEN
    for (i = 0; i < FEN.length() && FEN[i] != ' '; i++)
        ;
    for (i++; i < FEN.length() && FEN[i] != ' '; i++)
        ;
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

Move Engine::getUserMove() {
    std::string input;
    Move move;

    for (;;) {
        std::cout << "Enter your next move: ";
        std::getline(std::cin, input);

        if (validateMove(move, input))
            return move;
        else
            std::cout << "Entered move is invalid, try again" << '\n';
    }
}
bool Engine::validateMove(Move& move, std::string moveString) {
    if (moveString.size() < 4)
        return false;

    //extract start, end pos and promotionPiece as strings
    std::string startPosStr         = moveString.substr(0, 2);
    std::string endPosStr           = moveString.substr(2, 2);
    std::string promotionPieceStr   = moveString.substr(4);

    //extract start, end pos and promotionPiece as enums
    SquareIndex startPos            = SquareIndex(8 * (startPosStr[0] - 'a') + (startPosStr[1] - '1'));
    SquareIndex endPos              = SquareIndex(8 * (endPosStr[0]   - 'a') + (endPosStr[1]   - '1'));
    PieceType   promotionPiece      = INVALID;

    if (promotionPieceStr.size()) {
        switch (promotionPieceStr[0]) {
            case 'Q': case 'q': { promotionPiece = board->getWhiteTurn() ? WHITE_QUEEN  : BLACK_QUEEN;    break; }
            case 'R': case 'r': { promotionPiece = board->getWhiteTurn() ? WHITE_ROOK   : BLACK_ROOK;     break; }
            case 'B': case 'b': { promotionPiece = board->getWhiteTurn() ? WHITE_BISHOP : BLACK_BISHOP;   break; }
            case 'K': case 'k': { promotionPiece = board->getWhiteTurn() ? WHITE_KNIGHT : BLACK_KNIGHT;   break; }
            default:            { return false; } //user has given invalid promotion piece
        }
    }

    //check for a valid move with the given start pos and end pos
    std::vector<Move> moves = generateMoves(*board);
    for (auto& i : moves) {
        //if start and end positions don't match
        if (i.normalMove.startPos != startPos || i.normalMove.endPos != endPos)
            continue;

        bool isPromotionMove = i.flag == MoveType::PROMOTION;
        bool promotionPieceGiven = promotionPiece != INVALID;

        //if there a promotion piece is given when it shouldn't be or vice versa
        if (isPromotionMove != promotionPieceGiven)
            return false;

        //copy the move over
        move = Move(i);
        if (isPromotionMove)
            move.promotionMove.newPieceType = promotionPiece;

        //found the correct move
        return true;
    }

    //failed to find move with the same start pos and end pos
    return false;
}

GameState Engine::getCurrentGameState() {
    std::vector<Move> moves = generateMoves(*board);

    if (!moves.size())
        return isKingTargeted(*board) ? GameState::Checkmate : GameState::Stalemate;
    
    return GameState::Live;
}

uint64_t Engine::perft(int depth) {
    std::vector<Move> moves;
    uint64_t nodes = 0;

    if (depth == 0)
        return  1ULL;

    moves = generateMoves(*board);

    for (auto& i : moves) {
        board->makeMove(i);

        nodes += perft(depth -1);

        board->unMakeMove(i);
    }

    return nodes;
}

uint64_t Engine::perftDivide(int depth) {
    std::vector<Move> moves;
    std::vector<int> childMoveCount;
    uint64_t nodes = 0;

    if (depth == 0)
        return  1ULL;

    moves = generateMoves(*board);

    for (auto& i : moves) {
        board->makeMove(i);

        uint64_t childMoves = perftDivide(depth -1);
        nodes += childMoves;
        childMoveCount.push_back(childMoves);

        board->unMakeMove(i);
    }

    std::cout << "=============================== " << "depth: " << depth << " ===============================" << '\n';
    for (int i = 0; i < moves.size(); i++) {
        printMove(moves[i]);
        std::cout << "child moves: " << childMoveCount[i] << '\n' << '\n';
    }

    return nodes;
}

void Engine::runPerftTests() {
    board->setDefaultBoard();                                                                //position 1                    - passed perf(7), further depths unfeasable
    // parseFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");        //position 2                    - passed
    // parseFen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");                                   //position 3                    - passed
    // parseFen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");            //position 4                    - passed
        // parseFen("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ");       //alternate to position 4       - passed
    // parseFen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");                 //position 5                    - passed
    // parseFen("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");   //position 6                    - passed perf(6), further depths unfeasable

    for (int i = 1; i <= 15; i++)
        std::cout << "depth: " << i << " nodes: " << perft(i) << '\n';
}
