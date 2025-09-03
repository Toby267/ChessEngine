#include "bot/Bot.hpp"

#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>
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
const int Bot::NUM_THREADS = 1;//std::thread::hardware_concurrency();
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
    
    pVariation pvLineeee;
    negaMaxConcurrent(1, -INT_MAX, INT_MAX, pvLineeee, board);
    std::cout << "pls fucking work\n";
    return pvLineeee.moves[0];

    // Move move;
    // for (const std::string book : OPENING_BOOKS)
        // if (queryOpeningBook(book, move))
            // return move;

    nodesSearched = 0;
    searchDeadlineReached = false;
    searchDeadline = MAX_SEARCH_TIME_MS + std::chrono::high_resolution_clock::now();

    for (int i = 1;; i++) {
        pVariation pvLine;
        if (negaMaxConcurrent(i, -INT_MAX, INT_MAX, pvLine, board) == Eval::CHEKMATE_ABSOLUTE_SCORE)
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

int Bot::negaMaxConcurrent(int depth, int alpha, int beta, pVariation& parentLine, Board& b) {
    if (searchDeadlineReached || (++nodesSearched % SEARCH_TIMER_NODE_FREQUENCY == 0 && checkTimer())) return beta; //effectively snipping this branch like in alpha-beta

    if (depth == 0) return Eval::pestoEval(b);
    // if (depth == 0) return quiescence(alpha, beta, b);

    std::vector<Move> moves = MoveGeneration::generateMoves(board);
    if (!moves.size()) return Eval::terminalNodeEval(board);
    orderMoves(moves);

    pVariation childLine;

    //thread stuff
    std::vector<std::thread> threads;
    std::vector<std::promise<int>> evals(moves.size());
    std::vector<std::future<int>> evalsF(moves.size());

    for (int i = 0; i < evals.size(); i++) {
        evalsF[i] = evals[i].get_future();
    }

    for (int movesCompleted = 0;;) {
        while (threadsAvailable.try_acquire()) {
            mu.lock();
            int index = movesCompleted;
            movesCompleted++;
            if (index >= moves.size())
                goto Threading_Complete;
            std::cout << "index: " << index << '\n';
            mu.unlock();
            threads.emplace_back([&, this]() {
                assert(index < moves.size());
                Board bb(b);
                bb.makeMove(moves[index]);
                evals[index].set_value(negaMaxConcurrent(depth-1, -beta, -alpha, childLine, bb));
                threadsAvailable.release();
            });
        }

        mu.lock();
        int index = movesCompleted;
        movesCompleted++;
        if (index >= moves.size())
            goto Threading_Complete;
        std::cout << "index: " << index << '\n';
        mu.unlock();

        assert(index < moves.size());
        
        b.makeMove(moves[index]);
        evals[index].set_value(negaMaxConcurrent(depth-1, -beta, -alpha, childLine, b));
        b.unMakeMove(moves[index]);
    }

    Threading_Complete:
    mu.unlock();
    
    std::cout << "jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj\n";
    //wait for every worker to finish
    for (std::thread& t : threads) {
        t.join();
    }
    std::cout << "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n";

    /*
    Should i move this logic within the lambda for each thread?
    It'll definitely be more complex but will it be more efficient?
    Maybe the overhead of mutexes and memcpy will be too much?
    */
    /* can just do evals.max() to find the maximum value then compare that to beta and alpha without looping through each one */
    //handle data returned from recursive calls

    int index, max = INT_MIN;
    for (int i = 0; i < evalsF.size(); i++) {
        int val = evalsF[i].get();
        if (val > max) {
            max = val;
            index = i;
        }
    }

    std::cout << "llllllllllllllllllllllllllllllllllllllllllllllllllllllllllll\n";

    if (max >= beta) {
        return beta;
    }
    if (max > alpha) {
        alpha = max;

        parentLine.moves[0] = moves[index];
        memcpy(parentLine.moves+1, childLine.moves, childLine.moveCount * sizeof(Move));
        parentLine.moveCount = childLine.moveCount + 1;
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

int Bot::quiescence(int alpha, int beta, Board& b) {
    int staticEval = Eval::pestoEval(b);

    int bestValue = staticEval;
    if (bestValue >= beta)
        return bestValue;
    if  (bestValue > alpha)
        alpha = bestValue;

    std::vector<Move> moves = MoveGeneration::generateMoves(b);
    if (moves.size()) orderMovesQuiescence(moves);

    for (const Move& move : moves) {
        b.makeMove(move);

        int eval = -quiescence(-beta, -alpha, b);

        b.unMakeMove(move);

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
