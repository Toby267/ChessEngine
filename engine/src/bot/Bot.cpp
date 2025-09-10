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

Bot::Bot(Board& board) : board(board), SEARCH_TIMER_NODE_FREQUENCY(1024) {
    if (!isPestoInitialised) {
        isPestoInitialised = true;
        Eval::initPestoTables();
    }

    thinkTime = std::chrono::milliseconds(10000);
}
Bot::~Bot() {

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ---------------------------------------- [ GETTERS/SETTERS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Bot::setTimeLeftMs(int time) {
    timeLeftMs = time;
}

void Bot::setTimeIncrementMs(int time) {
    timeIncrement = time;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ----------------------------------------- [ PUBLIC METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

Move Bot::getBestMove() {
    forcedStop.store(false);
    
    float factor = timeLeftMs <= 45000 ? 0.02 : 0.08;
    thinkTime = std::chrono::milliseconds(int(factor * timeLeftMs));

    timeLeftMs -= thinkTime.count();
    timeLeftMs += timeIncrement;

    return calcBestMove();
}

Move Bot::getBestMove(int allocatedTime) {
    forcedStop.store(false);

    thinkTime = std::chrono::milliseconds(allocatedTime);

    timeLeftMs -= thinkTime.count();
    timeLeftMs += timeIncrement;

    return calcBestMove();
}

void Bot::reset() {
    principalVariation.moveCount = 0;
    movesOutOfBook = 0;
    movesPlayed = 0;
    timeLeftMs = 600000;
}

void Bot::stop() {
    forcedStop.store(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ---------------------------------------- [ PRIVATE METHODS ] ---------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Conducts searching and evaluation in order to find and return the predicted best possible move
 * given the current board state
 * 
 * @return the best move
 */
Move Bot::calcBestMove() {   
    nodesSearched = 0;
    searchDeadlineReached = false;
    searchDeadline = thinkTime + std::chrono::high_resolution_clock::now();
    
    movesPlayed++;
    
    Move move;
    for (const std::string book : OPENING_BOOKS)
        if (queryOpeningBook(book, move))
            return move;

    movesOutOfBook++;

    for (int i = 1;; i++) {
        pVariation pvLine;
        if (negaMax(i, -INT_MAX, INT_MAX, pvLine) == Eval::CHEKMATE_ABSOLUTE_SCORE)
            return pvLine.moves[0];
        if (searchDeadlineReached || forcedStop.load())
            return principalVariation.moves[0];

        principalVariation = pvLine;
    }
}

int Bot::negaMax(int depth, int alpha, int beta, pVariation& parentLine) {
    if (forcedStop.load()) return beta;
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
// * -------------------------------------- [ CONCURRENCY METHODS ] -------------------------------------- * //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Bot::negaMaxConcurrent(int depth, int alpha, int beta, pVariation& parentLine, Board b) {
    if (forcedStop.load()) return beta;
    if (searchDeadlineReached || (++nodesSearched % SEARCH_TIMER_NODE_FREQUENCY == 0 && checkTimer())) return beta; //effectively snipping this branch like in alpha-beta
    
    if (depth == 0) return quiescence(alpha, beta, b);

    std::vector<Move> moves = MoveGeneration::generateMoves(b);
    if (!moves.size()) return Eval::terminalNodeEval(b);
    orderMoves(moves);

    pVariation childLine;

    //thread stuff
    std::vector<std::future<int>> threads(moves.size());

    for (int i = 0, max = moves.size();;) {
        while (threadsAvailable.try_acquire()) {
            int index = i++;
            if (index >= max) break;
            Board bb(b);
            bb.makeMove(moves[index]);
            threads[index] = std::async(std::launch::async, [this, depth, alpha, beta, &childLine, bb](){
                int eval = -negaMaxConcurrent(depth-1, -beta, -alpha, childLine, bb);
                threadsAvailable.release();
                return eval;
            });
        }

        int index = i++;
        if (index >= max) break;
        b.makeMove(moves[index]);
        std::promise<int> val;
        threads[index] = val.get_future();
        val.set_value(-negaMaxConcurrent(depth-1, -beta, -alpha, childLine, b));
        b.unMakeMove(moves[index]);
    }

    //this is slower because it has to wait for all child threads to complete before alpha-beta pruning, i.e.: no pruning at all
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
