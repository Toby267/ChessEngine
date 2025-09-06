#include "bot/Bot.hpp"

#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <future>
#include <iostream>
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

Bot::Bot(Board& board) : boardRef(board), MAX_SEARCH_TIME_MS(10000), SEARCH_TIMER_NODE_FREQUENCY(1024) {
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
    nodesSearched = 0;
    searchDeadlineReached = false;
    searchDeadline = MAX_SEARCH_TIME_MS + std::chrono::high_resolution_clock::now();
    
    tree->reset(false);
    pVariation pvLineeee;
    negaMaxConcurrent(4, -INT_MAX, INT_MAX, pvLineeee, boardRef, tree);
    return pvLineeee.moves[0];

    // Move move;
    // for (const std::string book : OPENING_BOOKS)
        // if (queryOpeningBook(book, move))
            // return move;

    for (int i = 1;; i++) {
        pVariation pvLine;
        if (negaMaxConcurrent(i, -INT_MAX, INT_MAX, pvLine, boardRef, tree) == Eval::CHEKMATE_ABSOLUTE_SCORE)
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

    std::vector<Move> moves = MoveGeneration::generateMoves(boardRef);
    if (!moves.size()) return Eval::terminalNodeEval(boardRef);
    orderMoves(moves);

    pVariation childLine;

    for (const Move& move : moves) {
        boardRef.makeMove(move);

        int eval = -negaMax(depth-1, -beta, -alpha, childLine);
        
        boardRef.unMakeMove(move);

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

int Bot::negaMaxConcurrent(int depth, int alpha, int beta, pVariation& parentLine, Board b, concurrencyTreeNode* parentTerminate) {
    // if (searchDeadlineReached || (++nodesSearched % SEARCH_TIMER_NODE_FREQUENCY == 0 && checkTimer())) return beta; //effectively snipping this branch like in alpha-beta
    
    if (depth == 0) return quiescence(alpha, beta, b);

    std::vector<Move> moves = MoveGeneration::generateMoves(b);
    if (!moves.size()) return Eval::terminalNodeEval(b);
    orderMoves(moves);

    pVariation childLine;
    concurrencyTreeNode* childTerminate = parentTerminate->addChild(false);

    /*
    TODO:
        make alpha-beta pruning actually work:
            somehow integrate it into the for concurrency loop
            if val >= beta, delete all recursive threads, release their respective semaphores and return beta
            the rest of it can be after the concurrency loop

        to do this i need to:
            store a tree like data strucutre (as an attribute of this class) that stores references to the std::atomic<bool>& 's that you will pass through the recursion
            once one needs to change, call a function that will put an exclusive lock on a global std::shared_mutex, (stopping all threads from creating a new thread as they will need to put a 
            shared lock on it before creating new threads (they will also be checking its respective std::atomic<bool> here)), set the right bool to false, and propogate this through the tree
            setting all to false that need to be false.
        
            one thing that might make this more efficient (or less efficient, but probably more) is to have a separate std::shared_mutex for each node, and when a new thread is to be created, the
            thread doing the creation will need to acquire a shared lock for its parents mutex, its parents parents mutex, and so on. this will mean that only threads that need to be locked will be locked,
            and none other. although the overhead of multile mutexes might be greater than the overhead of stopping all threads with one shared mutex and quickly propogating.  
    */

    //thread stuff
    std::vector<std::future<int>> threads(moves.size());

    for (int i = 0, max = moves.size();;) {
        while (threadsAvailable.try_acquire()) {
            int index = i++;
            if (index >= max) break;
            Board bb(b);
            bb.makeMove(moves[index]);
            mu.lock_shared();
            threads[index] = std::async(std::launch::async, [this, depth, alpha, beta, &childLine, bb, &childTerminate](){
                int eval = -negaMaxConcurrent(depth-1, -beta, -alpha, childLine, bb, childTerminate);
                
                if (eval >= beta) {
                    //snip
                    mu.lock();
                    childTerminate->setTrue();
                    mu.unlock();
                    eval = beta;
                }
                
                threadsAvailable.release();
                return eval;
            });
            mu.unlock_shared();
        }

        int index = i++;
        if (index >= max) break;
        b.makeMove(moves[index]);
        std::promise<int> val;
        mu.lock_shared();
        threads[index] = val.get_future();
        int eval = -negaMaxConcurrent(depth-1, -beta, -alpha, childLine, b, childTerminate);
        if (eval >= beta) {
            mu.lock();
            childTerminate->setTrue();
            mu.unlock();
            eval = beta;
        }
        val.set_value(eval);
        mu.unlock_shared();
        b.unMakeMove(moves[index]);
    }

    for (int i = 0; i < threads.size(); i++) {
        int val = threads[i].get();

        if (val >= beta) {
            return beta;
        }
        if (val > alpha) {
            alpha = val;
            
            parentLine.moves[0] = moves[i];
            memcpy(parentLine.moves+1, childLine.moves, childLine.moveCount * sizeof(Move));
            parentLine.moveCount = childLine.moveCount + 1;
        }
    }

    return alpha;
}

//credit due to the chess programming wiki for this function
int Bot::quiescence(int alpha, int beta) {
    int staticEval = Eval::pestoEval(boardRef);

    int bestValue = staticEval;
    if (bestValue >= beta)
        return bestValue;
    if  (bestValue > alpha)
        alpha = bestValue;

    std::vector<Move> moves = MoveGeneration::generateMoves(boardRef);
    if (moves.size()) orderMovesQuiescence(moves);

    for (const Move& move : moves) {
        boardRef.makeMove(move);

        int eval = -quiescence(-beta, -alpha);

        boardRef.unMakeMove(move);

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
    
    std::vector<Move> moves = MoveGeneration::generateMoves(boardRef);
    std::vector<Move> bookMoves;

    for (const auto& m : moves) {
        boardRef.makeMove(m);

        std::string fen = boardRef.toFen();
        std::string line;

        book.clear();
        book.seekg(0);

        while (getline(book, line))
            if (fen == line.substr(0, fen.size()))
                if (std::count(bookMoves.begin(), bookMoves.end(), m) == 0)
                    bookMoves.push_back(m);

        boardRef.unMakeMove(m);
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
