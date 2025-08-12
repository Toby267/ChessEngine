#include "Engine.hpp"

#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "Bot.hpp"
#include "board/Board.hpp"
#include "board/BoardUtil.hpp"
#include "board/Move.hpp"
#include "board/moveGeneration/MoveGenerator.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ------------------------------------ [ CONSTRUCTORS/DESCTUCTOR ] ------------------------------------ * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

Engine::Engine() {    
    runPerftTests(2);
}
Engine::Engine(UserColour isBotWhite) : isBotWhite(isBotWhite){
    boardPositionCounter[board->getBitBoardsAsBitset()]++;
    playMatch();
}

Engine::~Engine() {
    delete board;
    delete bot;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ PUBLIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Plays a game of chess with the user through the CLI
 */
void Engine::playMatch() {
    for (;;) {
        printASCIIBoard();

        Move move = (board->getWhiteTurn() == isBotWhite) ? bot->getBestMove(*board) : getUserMove();
        board->makeMove(move);
        board->cleanup();
        
        gameState = getCurrentGameState();
        if (gameState != GameState::Live) break;
    }

    printASCIIBoard();

    if (gameState == GameState::Checkmate)
        std::cout << (board->getWhiteTurn() ? "Black Wins!" : "White Wins!") << '\n';
    else if (gameState == GameState::Stalemate)
        std::cout << "Draw by stalemate" << '\n';
    else if (gameState == GameState::DrawByRepetition)
        std::cout << "Draw by repetition" << '\n';
}

/**
 * Passes the fen to the board to parse
 * 
 * @param FEN the fen that is to be parsed
*/
void Engine::parseFen(const std::string& FEN) {
    board->parseFen(FEN);
    boardPositionCounter[board->getBitBoardsAsBitset()]++;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ---------------------------------------- [ PRIVATE METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Prints out the board using unicode characters for the chess pieces
 */
void Engine::printASCIIBoard() {    
    for (int rank = 7; rank >= 0; rank--) {
        std::cout << rank+1 << " ";
        for (int file = 0; file < 8; file++) {
            PieceType::Enum type = board->getType((SquareIndex)(8*file+rank));

            std::cout << PieceType::pieceChars[(int)type+1] << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "  A B C D E F G H" << "\n\n";
}

/**
 * Returns the Current state of the game whether it be live, checkmate, or any variation of a draw
 * 
 * @return the current game state
 */
GameState Engine::getCurrentGameState() {
    std::vector<Move> moves = generateMoves(*board);

    if (!moves.size())
        return isKingTargeted(*board) ? GameState::Checkmate : GameState::Stalemate;

    if (++boardPositionCounter[board->getBitBoardsAsBitset()] == 3)
        return GameState::DrawByRepetition;

    return GameState::Live;
}
/**
 * Gets, and validates the users move from the command line
 * 
 * @return the users move
 */
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

/**
 * Runs all perft tests outputting the results to the console
 */
void Engine::runPerftTests(int rigor) {
    std::array<std::string, 7> fens = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
        "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
        "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ",
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ",
        "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 "
    };

    std::vector<std::vector<long>> expectedResults = {
        { 20, 400, 8902, 197281,  4865609, 119060324, 3195901860, 84998978956, 2439530234167, 69352859712417, 2097651003696806, 62854969236701747, 1981066775000396239 },
        { 48, 2039, 97862, 4085603, 193690690, 8031647685 },
        { 14, 191, 2812, 43238, 674624, 11030083, 178633661, 3009794393  },
        { 6, 264, 9467, 422333, 15833292, 706045033 },
        { 6, 264, 9467, 422333, 15833292, 706045033 },
        { 44, 1486, 62379,  2103487,  89941194 },
        { 46, 2079, 89890, 3894594, 164075551, 6923051137, 287188994746, 11923589843526, 490154852788714 }
    };
    
    std::array<int, 7>   depths;
    if      (rigor == 1) depths = {5, 4, 6, 4, 4, 4, 4}; //in the millions of nodes
    else if (rigor == 2) depths = {6, 5, 7, 5, 5, 5, 5}; //in the tens-hundreds of millions of nodes
    else if (rigor == 3) depths = {7, 6, 8, 6, 6, 5, 6}; //in the billions of nodes


    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 7; i++) {
        parseFen(fens[i]);
        std::cout << "fen: " << fens[i] << '\n';

        for (int j = 1; j <= depths[i]; j++) {
            long nodes = perft(j);
            std::cout << "depth: " << j << " nodes: " << nodes << '\n';
            assert(nodes == expectedResults[i][j-1]);
        }

        std::cout << '\n';
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time ellapsed (miliseconds): " << duration.count() << '\n';
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ HELPER METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Validates, and parses a given move with the current board state
 * 
 * @param move the move reference to return the move to
 * @param moveString the move string to be parsed
 * @return whether or not it is a valid move
 */
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
    PieceType::Enum   promotionPiece      = PieceType::INVALID;

    if (promotionPieceStr.size()) {
        switch (promotionPieceStr[0]) {
            case 'Q': case 'q': { promotionPiece = board->getWhiteTurn() ? PieceType::WHITE_QUEEN  : PieceType::BLACK_QUEEN;    break; }
            case 'R': case 'r': { promotionPiece = board->getWhiteTurn() ? PieceType::WHITE_ROOK   : PieceType::BLACK_ROOK;     break; }
            case 'B': case 'b': { promotionPiece = board->getWhiteTurn() ? PieceType::WHITE_BISHOP : PieceType::BLACK_BISHOP;   break; }
            case 'K': case 'k': { promotionPiece = board->getWhiteTurn() ? PieceType::WHITE_KNIGHT : PieceType::BLACK_KNIGHT;   break; }
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
        bool promotionPieceGiven = promotionPiece != PieceType::INVALID;

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

/**
 * Runs perft up to a given depth on the current board
 * 
 * @param depth the depth to run up to
 * @return the total number of positions for the given depth
 */
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
/**
 * Runs perft up to a given depth on the current board, while printing out each node
 * 
 * @param depth the depth to run up to
 * @return the total number of positions for the given depth
 */
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
