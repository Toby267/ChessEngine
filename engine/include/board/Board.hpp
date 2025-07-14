#pragma once

#include <cstdint>
#include <string>
#include <array>

#include <board/BoardUtil.hpp>
#include <board/Move.hpp>

/**
 * Class representing the board and its relevent data, with varius functions for managing the board
 * 
 * Stores all 14 bitboards, indexed by the PieceType enum, using Little-Endian File-Rank Mapping/
 * Little-Endian Least-Significant-Rank Mapping as shown in the SquareIndex enum
 */
class Board {
private:
    //attribute default values are as if Board::resetBoard() has been called
    std::array<uint64_t, 14>    bitBoards        = {0};
    std::array<__uint128_t, 16> enPassantSquares = {0}; //1 in the right most bit means yes
                                                            //every time a move is played it is shifted left
                                                            //then shifted right when unplayed
    __uint128_t whiteCastleKing = 1, whiteCastleQueen = 1;  //1 in any bit means no
    __uint128_t blackCastleKing = 1, blackCastleQueen = 1;  //every time a castle or rook move is played it is shifted left
                                                            //then shifted right when unplayed
public:
    //constructors/destructor
    Board();
    ~Board();

    //getters/setters
    const std::array<uint64_t, 14>& getBitBoards() const;
    void setBitBoard(uint64_t val, PieceType type); //TODO: remove this method
    PieceType getType(SquareIndex index);

    //public methods
    void makeMove(const Move& move);
    void unMakeMove(const Move& move);

    void setDefaultBoard();
    void resetBoard();

    void parseFen(const std::string& FEN);

    void printDebugData();

private:
    //private methods
    void updateSpecialMoveStatus(const NormalMove& move);
    void checkDeadRook(const NormalMove& move);
    void checkDeadRook(const PromotionMove& move);

    void addPiece(PieceType type, SquareIndex index);
    void removePiece(PieceType type, SquareIndex index);
    void togglePiece(PieceType type, SquareIndex index);    

    void printBitBoardBin(PieceType board);
    void printBitBoardHex(PieceType board);

    //helper methods
    bool isWhite(SquareIndex index);
    bool isBlack(SquareIndex index);
    bool hasPiece(SquareIndex index);
};
