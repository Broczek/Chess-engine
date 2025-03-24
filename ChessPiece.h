#pragma once
#include "Board.h"
#include <string>

class Board;

class ChessPiece {
protected:
    int x, y;
    bool white;
    bool moved;

public:
    ChessPiece(int startX, int startY, bool isWhite);
    virtual ~ChessPiece() = default;

    virtual bool canMove(int newX, int newY, const Board& board) const = 0;
    virtual std::string getSymbol() const = 0;

    bool rusz(int newX, int newY, const Board& board);

    bool isWhite() const;
    void setPosition(int x, int y);
    bool hasMoved() const;
    void setMoved(bool moved);

    int getX() const;
    int getY() const;
};
