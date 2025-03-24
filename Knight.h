#pragma once
#include "ChessPiece.h"

class Knight : public ChessPiece {
public:
    Knight(int x, int y, bool white);
    bool canMove(int x, int y, const Board& board) const override;
    std::string getSymbol() const override;
};