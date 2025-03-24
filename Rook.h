#pragma once
#include "ChessPiece.h"

class Rook : public ChessPiece {
public:
    Rook(int x, int y, bool white);
    bool canMove(int newX, int newY, const Board& board) const override;
    std::string getSymbol() const override;
};
