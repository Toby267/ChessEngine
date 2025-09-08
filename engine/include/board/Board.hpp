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
    std::array<int, 64> mailBoxBoard{};

    std::array<__uint128_t, 16> enPassantData{};    //1 in the right most bit means yes
                                                    //every time a move is played it is shifted left
                                                    //then shifted right when unplayed
    std::array<__uint128_t, 4> castleData{};        //1 in any bit means no
                                                    //every time a castle or rook move is played it is shifted left
                                                    //then shifted right when unplayed
    WhiteTurn whiteTurn = true;
    int drawMoveCounter = 0;

public:
    //constructors/destructor
    Board();
    ~Board();

    //getters/setters
    const std::array<uint64_t, 14>& getBitBoards() const;
    const std::array<int, 64>& getMailboxBoard() const;
    const std::array<__uint128_t, 4>& getCastleData() const;
    const std::array<__uint128_t, 16>& getEnPassantData() const;
    PieceType::Enum getType(SquareIndex index) const;
    WhiteTurn getWhiteTurn() const;
    
    //public methods
    void makeMove(const Move& move);
    void unMakeMove(const Move& move);

    void cleanup();

    void setDefaultBoard();
    void resetBoard();

    void parseFen(const std::string& FEN);
    std::string toFen();

private:
    //private methods
    void updateSpecialMoveStatus(const Move& move);

    void addPiece(PieceType::Enum type, SquareIndex index);
    void removePiece(PieceType::Enum type, SquareIndex index);
    void togglePiece(PieceType::Enum type, SquareIndex index);    

    void printBitBoardBin(PieceType::Enum board);
    void printBitBoardHex(PieceType::Enum board);

    //helper methods
    bool hasPiece(SquareIndex index) const;
    bool isWhite(SquareIndex index) const;
    bool isBlack(SquareIndex index) const;
};
