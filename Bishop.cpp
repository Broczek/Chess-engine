#include "Bishop.h"
#include "Board.h"
#include <cstdlib>

Bishop::Bishop(int x, int y, bool white) : ChessPiece(x, y, white) {}

bool Bishop::canMove(int newX, int newY, const Board& board) const {
    if (abs(newX - x) != abs(newY - y)) return false;

    int stepX = (newX - x)/abs(newX - x);
    int stepY = (newY - y)/abs(newY - y);

    int currX = x + stepX, currY = y + stepY;

    while (currX != newX && currY != newY) {
        if (!board.isEmpty(currX, currY)) return false;
        currX += stepX;
        currY += stepY;
    }

    return board.isEmpty(newX, newY) || board.isOpponent(newX, newY, white);
}

std::string Bishop::getSymbol() const { 
    return white ? "♝" : "♗"; 
}

int Bishop::getTypeIndex() const { return 3; }
