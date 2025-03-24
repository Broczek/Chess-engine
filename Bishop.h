#pragma once
#include "ChessPiece.h"

class Bishop : public ChessPiece {
public:
    Bishop(int x, int y, bool white);
    bool canMove(int x, int y, const Board& board) const override;
    std::string getSymbol() const override;
};