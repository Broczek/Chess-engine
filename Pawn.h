#pragma once
#include "ChessPiece.h"

class Pawn : public ChessPiece {
public:
    Pawn(int x, int y, bool white);
    bool canMove(int x, int y, const Board& board) const override;
    std::string getSymbol() const override;
    int getTypeIndex() const override;
};