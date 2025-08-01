#pragma once

#include <cstdint>
#include <string>
#include <array>

#include "BoardUtil.hpp"
#include "Move.hpp"

/**
 * Class representing the board and its relevent data, with varius functions for managing the board
 * 
 * Stores all 14 bitboards, indexed by the PieceType enum, using Little-Endian File-Rank Mapping/
 * Little-Endian Least-Significant-Rank Mapping as shown in the SquareIndex enum
 */
class Board {
private:
    //attribute default values are as if Board::resetBoard() has been called
    std::array<uint64_t, 14> bitBoards{};

    std::array<__uint128_t, 16> enPassantData{};    //1 in the right most bit means yes
                                                    //every time a move is played it is shifted left
                                                    //then shifted right when unplayed
    std::array<__uint128_t, 4> castleData{};        //1 in any bit means no
                                                    //every time a castle or rook move is played it is shifted left
                                                    //then shifted right when unplayed

public:
    //constructors/destructor
    Board();
    ~Board();

    //getters/setters
    const std::array<uint64_t, 14>& getBitBoards() const;
    const std::array<__uint128_t, 4>& getCastleData() const;
    const std::array<__uint128_t, 16>& getEnPassantData() const;
    PieceType getType(SquareIndex index) const;
    
    //public methods
    void makeMove(const Move& move);
    void unMakeMove(const Move& move);

    void setDefaultBoard();
    void resetBoard();

    void parseFen(const std::string& FEN);

    void printDebugData();

private:
    //private methods
    void updateSpecialMoveStatus(const Move& move);

    void addPiece(PieceType type, SquareIndex index);
    void removePiece(PieceType type, SquareIndex index);
    void togglePiece(PieceType type, SquareIndex index);    

    void printBitBoardBin(PieceType board);
    void printBitBoardHex(PieceType board);

    //helper methods
    bool isWhite(SquareIndex index) const;
    bool isBlack(SquareIndex index) const;
    bool hasPiece(SquareIndex index);
};
