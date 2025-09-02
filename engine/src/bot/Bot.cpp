#include "bot/Bot.hpp"

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <future>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>
#include <chrono>

#include "board/Board.hpp"
#include "board/Move.hpp"
#include "bot/PrincipalVariation.hpp"
#include "moveGeneration/MoveGenerator.hpp"
#include "bot/Eval.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ STATIC MEMBERS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Bot::isPestoInitialised = false;
const int Bot::NUM_THREADS = std::thread::hardware_concurrency();
std::counting_semaphore<> Bot::threadsAvailable(NUM_THREADS);

const std::string Bot::OPENING_BOOKS[] = {
    "opening_book_21_moves.epd","opening_book_16_moves.epd", "opening_book_14_moves.epd", "opening_book_13_moves.epd", "opening_book_12_moves.epd", "opening_book_11_moves.epd",
    "opening_book_10_moves.epd", "opening_book_9_moves.epd", "opening_book_8_moves.epd", "opening_book_7_moves.epd", "opening_book_6_moves.epd", "opening_book_5_moves.epd",
    "opening_book_4_moves.epd", "opening_book_3_moves.epd"
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ------------------------------------ [ CONSTRUCTORS/DESCTUCTOR ] ------------------------------------ * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

Bot::Bot(Board& board) : board(board), MAX_SEARCH_TIME_MS(10000), SEARCH_TIMER_NODE_FREQUENCY(1024) {
    if (!isPestoInitialised) {
        isPestoInitialised = true;
        Eval::initPestoTables();
    }
}
Bot::~Bot() {

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ PUBLIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Conducts searching and evaluation in order to find and return the predicted best possible move
 * given the current board state
 * 
 * @return the best move
 */
Move Bot::getBestMove() {
    std::cout << "threads: " << NUM_THREADS << '\n';
    
    Move move;
    for (const std::string book : OPENING_BOOKS)
        if (queryOpeningBook(book, move))
            return move;

    nodesSearched = 0;
    searchDeadlineReached = false;
    searchDeadline = MAX_SEARCH_TIME_MS + std::chrono::high_resolution_clock::now();

    for (int i = 1;; i++) {
        pVariation pvLine;
        if (negaMax(i, -INT_MAX, INT_MAX, pvLine) == Eval::CHEKMATE_ABSOLUTE_SCORE)
            return pvLine.moves[0];
        if (searchDeadlineReached)
            return principalVariation.moves[0];

        principalVariation = pvLine;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ---------------------------------------- [ PRIVATE METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Bot::negaMax(int depth, int alpha, int beta, pVariation& parentLine) {
    if (searchDeadlineReached || (++nodesSearched % SEARCH_TIMER_NODE_FREQUENCY == 0 && checkTimer())) return beta; //effectively snipping this branch like in alpha-beta
    
    if (depth == 0) return quiescence(alpha, beta);

    std::vector<Move> moves = MoveGeneration::generateMoves(board);
    if (!moves.size()) return Eval::terminalNodeEval(board);
    orderMoves(moves);

    pVariation childLine;

    for (const Move& move : moves) {
        board.makeMove(move);

        int eval = -negaMax(depth-1, -beta, -alpha, childLine);
        
        board.unMakeMove(move);

        if (eval >= beta) {
            return beta;
        }
        if (eval > alpha) {
            alpha = eval;

            parentLine.moves[0] = move;
            memcpy(parentLine.moves+1, childLine.moves, childLine.moveCount * sizeof(Move));
            parentLine.moveCount = childLine.moveCount + 1;
        }
    }

    return alpha;
}

void Bot::negaMaxConcurrentPromoise(int depth, int alpha, int beta, pVariation& parentLine, Board* b, std::promise<int>) {}

int Bot::negaMaxConcurrent(int depth, int alpha, int beta, pVariation& parentLine, Board* b) {
    if (searchDeadlineReached || (++nodesSearched % SEARCH_TIMER_NODE_FREQUENCY == 0 && checkTimer())) return beta; //effectively snipping this branch like in alpha-beta
    
    if (depth == 0) return quiescence(alpha, beta);

    std::vector<Move> moves = MoveGeneration::generateMoves(board);
    if (!moves.size()) return Eval::terminalNodeEval(board);
    orderMoves(moves);

    pVariation childLine;

    std::vector<std::future<int>> evals;
    std::vector<int> intEvals;
    std::vector<std::promise<int>> evalPromises;
    std::vector<std::thread> threads;

    /*
    TODO:   don't know how to pass in the board to new threads, should it be a pointer? and reference? a normal variable?
                                                                should it be heap allocated? stack allocated?
            all other arguments are fine
    
    TODO:   how do i make and unmake moves for the new boards?
    
    TODO:   use std::async or std::promise to get the return value from the concurrent calls
    */

    for (int movesLeft = moves.size(); movesLeft > 0;) {
        while (threadsAvailable.try_acquire()) {
            //need to make and unmake the move

            //method 1:
            evals.push_back(std::async(std::launch::async, &Bot::negaMaxConcurrent, this, depth-1, -beta, -alpha, std::ref(childLine), b));
            
            //method 2:
            evals.push_back(std::async(std::launch::async, [&, this]() {
                int i = negaMaxConcurrent(depth-1, -beta, -alpha, childLine, b);
                threadsAvailable.release();
                return 1;
            }));

            //method 3:
            threads.emplace_back([&, this]() {
                intEvals.push_back(negaMaxConcurrent(depth-1, -beta, -alpha, childLine, b));
                threadsAvailable.release();
            });

            //method 4:
            threads.emplace_back([&, this]() {
                negaMaxConcurrentPromoise(depth-1, -beta, -alpha, childLine, b, std::move(evalPromises[moves.size()-movesLeft]));
                threadsAvailable.release();
            });

            movesLeft--;
        }
        //need to make and unmake the move
        intEvals.push_back(negaMaxConcurrent(depth-1, -beta, -alpha, childLine, b));
        movesLeft--; //what if a different thread completes first? then the order of evals won't match the order of moves...
        
        for (std::thread& t : threads) {
            t.join();
            threadsAvailable.release();//use this for method 1 (any method that doesn't have a lambda for this)
        }
    }

    for (int i = 0; i < moves.size(); i++) {
        if (intEvals[i] >= beta) {
            return beta;
        }
        if (intEvals[i] > alpha) {
            alpha = intEvals[i];

            parentLine.moves[0] = moves[i];
            memcpy(parentLine.moves+1, childLine.moves, childLine.moveCount * sizeof(Move));
            parentLine.moveCount = childLine.moveCount + 1;
        }
    }

    return alpha;
}

//credit due to the chess programming wiki for this function
int Bot::quiescence(int alpha, int beta) {
    int staticEval = Eval::pestoEval(board);

    int bestValue = staticEval;
    if (bestValue >= beta)
        return bestValue;
    if  (bestValue > alpha)
        alpha = bestValue;

    std::vector<Move> moves = MoveGeneration::generateMoves(board);
    if (moves.size()) orderMovesQuiescence(moves);

    for (const Move& move : moves) {
        board.makeMove(move);

        int eval = -quiescence(-beta, -alpha);

        board.unMakeMove(move);

        if (eval >= beta)
            return eval;
        if (eval > bestValue)
            bestValue = eval;
        if (eval > alpha)
            alpha = eval;
    }

    return bestValue;
}

bool Bot::queryOpeningBook(std::string bookName, Move& move) {
    std::ifstream book(RESOURCES_PATH + bookName);
    if (!book.is_open()) return false;
    
    std::vector<Move> moves = MoveGeneration::generateMoves(board);
    std::vector<Move> bookMoves;

    for (const auto& m : moves) {
        board.makeMove(m);

        std::string fen = board.toFen();
        std::string line;

        book.clear();
        book.seekg(0);

        while (getline(book, line))
            if (fen == line.substr(0, fen.size()))
                if (std::count(bookMoves.begin(), bookMoves.end(), m) == 0)
                    bookMoves.push_back(m);

        board.unMakeMove(m);
    }

    if (bookMoves.empty())
        return false;

    srand(time(0));
    move = bookMoves[rand()%bookMoves.size()];
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ HELPER METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Bot::orderMoves(std::vector<Move>& moves) {
    for (Move& m : moves) {
        for (int i = 0; i < principalVariation.moveCount; i++) {
            if (m == principalVariation.moves[i]) {
                m.heuristic *= 10;
                break;
            }
        }
    }

    std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b){
        return a.heuristic > b.heuristic;
    });
}

void Bot::orderMovesQuiescence(std::vector<Move>& moves) {
    moves.erase(std::remove_if(moves.begin(), moves.end(), [](const Move& m) {
        return m.heuristic <= 3;
    }), moves.end());

    std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b){
        return a.heuristic > b.heuristic;
    });
}

bool Bot::checkTimer() {
    return (searchDeadlineReached = std::chrono::high_resolution_clock::now() > searchDeadline);
}
