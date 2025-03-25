#include "Queen.h"
#include "Board.h"
#include <cmath>

Queen::Queen(int x, int y, bool white) : ChessPiece(x, y, white) {}

bool Queen::canMove(int newX, int newY, const Board& board) const {
    int dx = abs(x - newX);
    int dy = abs(y - newY);

    if (!(dx == dy || x == newX || y == newY)) return false;

    int stepX = (newX - x) ? ((newX - x) / abs(newX - x)) : 0;
    int stepY = (newY - y) ? ((newY - y) / abs(newY - y)) : 0;

    for (int ix = x + stepX, iy = y + stepY; ix != newX || iy != newY; ix += stepX, iy += stepY)
        if (!board.isEmpty(ix, iy)) return false;

    return board.isEmpty(newX, newY) || board.isOpponent(newX, newY, white);
}

std::string Queen::getSymbol() const { 
    return white ? "♛" : "♕"; 
}

int Queen::getTypeIndex() const { return 1; }